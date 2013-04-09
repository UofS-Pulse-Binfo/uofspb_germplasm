<?php

/**
 * Field handler allowing all relationships for a given stock to be displayed in a single cell
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
class views_handler_field_stockrel_all extends tripal_views_handler_field_aggregate {
  function init(&$view, $options) {
    parent::init($view, $options);

    if ($this->table != 'stock_relationship') {
      $this->table = 'stock_relationship';
      $this->field = 'all';
      $this->field_alias = 'stock_relationship_all';
      $this->additional_fields['stock_relationship_all'] = array('table' => 'stock_relationship', 'field' => 'all');

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
    $this->query->ensure_table('stock_relationship');
    $this->add_additional_fields();

    // Add a join between stock_relationship and stock for the subject
    // We want this to be added within the stock_relationship subquery
    $table = $this->query->get_table_info('stock_relationship');
    $table['join']->additional_joins['subject'] = array(
      'table' => 'stock',
      'table_alias' => 'subject',
      'field' => 'stock_id',
      'left_table' => 'stock_relationship',
      'left_field' => 'subject_id',
    );

    // Add a join between stock_relationship and stock for the object
    // We want this to be added within the stock_relationship subquery
    $table = $this->query->get_table_info('stock_relationship');
    $table['join']->additional_joins['object'] = array(
      'table' => 'stock',
      'table_alias' => 'object',
      'field' => 'stock_id',
      'left_table' => 'stock_relationship',
      'left_field' => 'object_id',
    );

  }

	function pre_render($values) {
		$this->aliases['relationships'] = 'stock_relationship_all';
		$this->aliases['stock_id'] = 'stock_id';
		$this->field_alias = $this->aliases['relationships'];

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
      $items = $values->{$this->aliases['relationships']};
      if (empty($items)) { $items = array(); }
      $rendered_items = array();
      foreach ($items as $k => $item) {
        if (!is_int($k)) {
          continue;
        }
        $rendered_items[] =  $item['subject_name'].' '.$item['cvterm_name'].' '.$item['object_name'];
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
      if (empty($values->{$this->field_alias})) {
        $values->{$this->field_alias} = '';
      }
      return $this->parent_render($values);
    }
  }
}

