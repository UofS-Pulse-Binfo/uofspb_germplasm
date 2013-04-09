<?php

/**
 * Field handler allowing all relationships of a specified type for a given stock to be displayed in
 * a single cell
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
class views_handler_field_stockrel_by_type extends tripal_views_handler_field_aggregate {
  function init(&$view, $options) {
    parent::init($view, $options);

    if ($this->table != 'stock_relationship') {
      $this->table = 'stock_relationship';
      $this->field = 'all';
      $this->field_alias = 'stock_relationship_all';
      $this->additional_fields['stock_relationship_all'] = array('table' => 'stock_relationship', 'field' => 'all');
      $this->additional_fields['stock_stock_id'] = array('table' => 'stock', 'field' => 'stock_id');

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
    $options['stockrel_type_ids'] = array('default' => array());
    $options['stockrel_display_options'] = array('default' => array('subject','type','object'));
    return $options;
  }

  /**
   * Provide "link to term" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    unset($form['format']);

    $form['stockrel_display_parts'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Display Relationship Parts'),
      '#description' => t('Check each part of the relationship you want displayed where the part '
        .'of a relationship are: \<Subject\> \<Relationship Type\> \<Object\>. '
        .'For example, with the relationship Sarah is the maternal parent of Fred '
        .'if you checked only Object then "Fred" would be displayed.'),
      '#options' => array(
        'subject' => 'Subject',
        'type' => 'Relationship Type',
        'object' => 'Object',
      ),
      '#default_value' => array($this->options['stockrel_display_parts']['subject'], $this->options['stockrel_display_parts']['type'], $this->options['stockrel_display_parts']['object']),
    );

    $form['stockrel_display_rels'] = array(
      '#type' => 'radios',
      '#title' => t('Display Relationships Where'),
      '#description' => t('Only relationships where the selected criteria is met will be shown. '
        .'The parts of a relationship are: \<Subject\> \<Relationship Type\> \<Object\>. '
        .'For example, with the relationships Sarah is the maternal parent of Fred and '
        .'Fred is the paternal_parent of Max where Fred is the current stock, '
        .'if you selected "Current Stock is the Object" only Sarah is the maternal parent of Fred'
        .' would be displayed.'),
      '#options' => array(
        'subject' => 'Current Stock is the Subject',
        'object' => 'Current Stock is the Object',
        'all' => 'Current Stock is the Subject and/or Object',
      ),
      '#default_value' => $this->options['stockrel_display_rels'],
    );

    $options = tripal_cv_get_cvterm_options( variable_get('chado_stock_relationship_cv', 'null') );
    $form['stockrel_type_ids'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Relationship Types'),
      '#options' => $options,
      '#default_value' => $this->options['stockrel_type_ids'],
    );
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
    if (!isset($this->aliases['field_handler'])) {
      $this->aliases['field_handler'] = 'stockrel_by_type_' . $this->position;
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

    if (!empty($this->options['stockrel_type_ids'])) {
      $relationships2keep = array_filter($this->options['stockrel_type_ids']);
    }

    //for each stock in this view page
    foreach ($values as $vk => $result) {
      if (!empty($result->{$this->aliases['relationships']})) {

        // all relationships including the current stock
        $relationships = $result->{$this->aliases['relationships']};
        foreach ($relationships as $rk => $relationship) {
          // perform filtering------
          //type
          if (!empty($relationships2keep) AND !in_array($relationship['type_id'], $relationships2keep)) {
            continue;
          }

          //"Display Relationships Where" criteria
          if (preg_match('/subject/', $this->options['stockrel_display_rels'])) {
            if ($result->stock_stock_id != $relationship['subject_id']) {
              continue;
            }
          } elseif (preg_match('/object/', $this->options['stockrel_display_rels'])) {
            if ($result->stock_stock_id != $relationship['object_id']) {
              continue;
            }
          }

          // Add relationship to the list of items to be rendered
          $values[$vk]->{$this->aliases['relationships']}[$this->aliases['field_handler']][$rk] = $relationship;
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
      $items = $values->{$this->aliases['relationships']}[$this->aliases['field_handler']];
      if (empty($items)) { $items = array(); }
      $rendered_items = array();
      foreach ($items as $k => $item) {
        if (!is_int($k)) {
          continue;
        }

        $text = array();
        // Render Parts
        if ($this->options['stockrel_display_parts']['subject']) {
          $text[] = $item['subject_name'];
        }
        if ($this->options['stockrel_display_parts']['type']) {
          $text[] = $item['cvterm_name'];
        }
        if ($this->options['stockrel_display_parts']['object']) {
          $text[] = $item['object_name'];
        }

        $rendered_items[] = implode(' ', $text);
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

