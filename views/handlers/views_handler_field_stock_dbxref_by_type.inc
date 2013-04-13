<?php

/**
 * Field handler allowing all database references of a specified db for a given stock to be
 * displayed in a single cell.
 *
 * This handler only deals with database references joined to the stock through stock_dbxref. For
 * the database reference joined to the stock by stock.dbxref_id simply join to the dbxref table.
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
class views_handler_field_stock_dbxref_by_type extends tripal_views_handler_field_aggregate {
  function init(&$view, $options) {
    parent::init($view, $options);

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

    // Boolean to determine whether
    //    TRUE =>  value (property type)      -more than one property type displayed
    //    FALSE => or just value is rendered  -only 1 porperty type is displayed
    $this->display_type = TRUE;

  }

  function option_definition() {
    $options = parent::option_definition();
    $options['stockprop_type_ids'] = array('default' => array());
		$options['display_options'] = array('default' => array('accession','db_name','link'));
    return $options;
  }

  /**
   * Provide "link to term" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    unset($form['format']);

		$form['display_options'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Display DB Reference Parts'),
			'#description' => t('Check each part of the database reference you want displayed where the parts '
				.'of a database relationship are: <Accession> (<Datbase Name>). You can also specify whether the accession '
				.'should be linked to the record in the database by checking "Link to External Record".'),
			'#options' => array(
				'accession' => 'Accession',
				'db_name' => 'Database Name',
				'link' => 'Link to External Record',
			),
			'#default_value' => array($this->options['display_options']['accession'], $this->options['display_options']['db_name'], $this->options['display_options']['link']),
		);

    $db_options = tripal_db_get_db_options();
	  $db_options[0] = 'Select a Database';
	  ksort($db_options);
    $form['db_ids'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Database'),
      '#options' => $db_options,
      '#default_value' => $this->options['db_ids'],
    );
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
  	unset($this->options['format']['format_string']);
		$this->aliases['dbxrefs'] = 'stock_dbxref_all';
		$this->aliases['stock_id'] = 'stock_id';
		$this->field_alias = $this->aliases['dbxrefs'];
    if (!isset($this->aliases['field_handler'])) {
      $this->aliases['field_handler'] = 'stockdbxref_by_type_' . $this->position;
    }

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

    if (!empty($this->options['db_ids'])) {
			$dbs2keep = array_filter($this->options['db_ids']);
		}

		//for each stock in this view page
		foreach ($values as $vk => $result) {
			if (!empty($result->{$this->aliases['dbxrefs']})) {

				// all dbxrefs including the current stock
				$dbxrefs = $result->{$this->aliases['dbxrefs']};
				foreach ($dbxrefs as $rk => $dbxref) {

					// perform filtering
					if (!empty($dbs2keep)) {
						if (!in_array($dbxref['db_db_id'], $dbs2keep)) {
							continue;
						}
					}

					// Add dbxref to the list of items to be rendered
					$values[$vk]->{$this->aliases['dbxrefs']}[$this->aliases['field_handler']][$rk] = $dbxref;
				}
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
      $items = $values->{$this->aliases['dbxrefs']}[$this->aliases['field_handler']];
      if (empty($items)) { $items = array(); }
      $rendered_items = array();
      foreach ($items as $k => $item) {
        if (!is_int($k)) {
          continue;
        }

        $text = array();
        if ($this->options['display_options']['link'] AND !empty($item['db_urlprefix'])) {
          if ($this->options['display_options']['accession']) {
            $text[] = l($item['dbxref_accession'],$item['db_urlprefix'].$item['dbxref_accession']);
          } else {
            $text[] = l('External Record',$item['db_urlprefix'].$item['dbxref_accession']);
          }
        } else {
          if ($this->options['display_options']['accession']) {
            $text[] = $item['dbxref_accession'];
          }
        }


        if ($this->options['display_options']['db_name']) {
          $text[] = $item['db_name'];
        }

        if (sizeof($text) > 1) {
          $rendered_items[] = $text[0].' ('.$text[1].')';
        } else {
          $rendered_items[] = $text[0];
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

