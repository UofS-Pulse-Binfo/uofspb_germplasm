<?php

/**
 * Field handler allowing all database references for a given stock to be displayed in a single cell
 *
 * This handler only deals with database references joined to the stock through stock_dbxref. For
 * the database reference joined to the stock by stock.dbxref_id simply join to the dbxref table.
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
class views_handler_field_stock_dbxref_all extends tripal_views_handler_field_aggregate {

  function init(&$view, $options) {
    parent::init(&$view, $options);

    if ($this->table != 'stock_dbxref') {
      $this->table = 'stock_dbxref';
      $this->field = 'all';
      $this->field_alias = 'stock_dbxref_all';
      $this->additional_fields['stock_dbxref_all'] = array('table' => 'stock_dbxref', 'field' => 'all');

      $this->chado_table_description = tripal_core_get_chado_table_schema($this->table);
      foreach ($this->chado_table_description['foreign keys'] as $defn) {
        if ($defn['table'] != $this->view->base_table) {
          $join_table = tripal_core_get_chado_table_schema($defn['table']);
          foreach ($join_table['fields'] as $fname => $f) {
            $this->chado_table_description['fields'][$defn['table'] . '_' . $fname] = $f;
          }
        }
      }
    }
  }

  /**
   * Add this term to the query
   */
  function query() {
    $this->query->ensure_table('stock_dbxref');
    $this->add_additional_fields();

    // Add a join between stock_dbxref and db
    // We want this to be added within the stock_dbxref subquery
    $table = $this->query->get_table_info('stock_dbxref');
    $table['join']->additional_joins['db'] = array(
      'table' => 'db',
      'field' => 'db_id',
      'left_table' => 'fk_dbxref_dbxref_id',
      'left_field' => 'db_id',
    );
  }

	function pre_render($values) {
		$this->aliases['dbxrefs'] = 'stock_dbxref_all';
		$this->aliases['stock_id'] = 'stock_id';
		$this->field_alias = $this->aliases['dbxrefs'];

    // further check the results to see if this field is a postgresql array
    $this->aggregated = chado_wrapper_is_aggregated_by_result($this, $values);
    if ($this->aggregated) {
      // Split Aggregated Results
      chado_wrapper_split_array_agg_results($this, $values);

      // Now each result is of the following form:
      // stockprop_id::16554,stock_id::12037,type_id::3650,value::Sm2008-P13,rank::1...
      // we need to split it further
      foreach ($values as $k => $v) {
        $this->split_aggregated_result_with_keys($values[$k]->{$this->field_alias});
      }
    }
	}

  /**
   * Render the field.
   *
   * Note: Checks to see if we have an array or simple field. If we have an array, then
   *   split it up and render each part using the parent render functionality.
   *
   * @param $values
   *   The values retrieved from the database.
   */
  function render($values) {

    // If it's aggregated (an array), then render each part
    // using the parent render functionality
    if ($this->aggregated) {
      $items = $values->{$this->aliases['dbxrefs']};
      if (empty($items)) { $items = array(); }
      $rendered_items = array();
      foreach ($items as $k => $item) {
        if (!is_int($k)) {
          continue;
        }

        if (!empty($item['db_urlprefix'])) {
          $rendered_items[] = l($item['dbxref_accession'],$item['db_urlprefix'].$item['dbxref_accession']).' ('.$item['db_name'].')';
        } else {
          $rendered_items[] = $item['dbxref_accession'].' ('.$item['db_name'].')';
        }

      }

      if ($this->options['type'] == 'separator') {
        return implode(check_plain($this->options['separator']), $rendered_items);
      }
      else {
        return theme('item_list', $rendered_items, NULL, $this->options['type']);
      }
    // Otherwise it is not aggragated
    // Just render like the default handler would
    }
    else {
      if (empty($values->{$this->aliases['dbxrefs']})) {
        $values->{$this->aliases['dbxrefs']} = '';
      }
      return $this->parent_render($values);
    }
  }
}

