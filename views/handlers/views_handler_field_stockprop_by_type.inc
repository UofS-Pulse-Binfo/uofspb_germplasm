<?php

/**
 * Field handler allowing all properties of a specified type for a given stock to be displayed in a
 * single cell
 *
 * @ingroup tripal_stock
 * @ingroup views_field_handlers
 */
class views_handler_field_stockprop_by_type extends tripal_views_handler_field_aggregate {

  function init(&$view, $options) {
    parent::init(&$view, $options);

    if ($this->table != 'stockprop') {
      $this->table = 'stockprop';
      $this->field = 'all';
      $this->field_alias = 'stockprop_all';
      $this->additional_fields['stockprop_all'] = array('table' => 'stockprop', 'field' => 'all');

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

  function option_definition() {
    $options = parent::option_definition();
    $options['stockprop_type_ids'] = array('default' => array());
    $options['stockprop_display_options'] = array('default' => array('type', 'value'));
    return $options;
  }

  // Provide "link to term" option.
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    unset($form['format']);

    $form['stockprop_display_options'] = array(
      '#prefix' => '<div><div id="edit-options-display_options">',
      '#suffix' => '</div></div>',
      '#type' => 'checkboxes',
      '#title' => t('Display Relationship Parts'),
      '#description' => t('Check each part of the property you want displayed where the parts '
        .'of a property are: <type> and <value>. '
        .'For example, with the property temperature 54 degrees Celcius the type=temperature and the value=54 degrees Celcius. '
        .'Thus, if you checked only value then "54 degrees Celcius" would be displayed.'),
      '#options' => array(
        'type' => 'Type',
        'value' => 'Value',
      ),
      '#default_value' => array($this->options['stockprop_display_options']['type'], $this->options['stockprop_display_options']['value']),
    );

    $options = tripal_cv_get_cvterm_options( variable_get('chado_stock_prop_types_cv', 'null') );
    $form['stockprop_type_ids'] = array(
      '#prefix' => '<div><div id="edit-options-stockprop_type_ids">',
      '#suffix' => '</div></div>',
      '#type' => 'checkboxes',
      '#title' => t('Property Types'),
      '#options' => $options,
      '#default_value' => $this->options['stockprop_type_ids'],
    );
  }

  // Add this term to the query
  function query() {
    $this->query->ensure_table('stockprop');
    $this->add_additional_fields();
  }

  function pre_render($values) {
    unset($this->options['format']['format_string']);
    $this->aliases['properties'] = 'stockprop_all';
    if (!isset($this->aliases['field_handler'])) {
      $this->aliases['field_handler'] = 'stockprop_by_type_' . $this->position;
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

    if (!empty($this->options['stockprop_type_ids'])) {
      $types2keep = array_filter($this->options['stockprop_type_ids']);
    }
    //for each stock in this view page
    foreach ($values as $vk => $results) {
      if (!empty($results->{$this->field_alias})) {
        // all properties for the current stock
        $properties = $results->{$this->field_alias};
        foreach ($properties as $rk => $property) {
          $property = (object) $property;
          // perform filtering
          if (!empty($types2keep)) {
            if (!in_array($property->type_id, $types2keep)) {
              continue;
            }
          }

          // Add property to the list of items to be rendered
          $this->items[$property->stock_id][$property->stockprop_id]['stock_id'] = $property->stock_id;
          $this->items[$property->stock_id][$property->stockprop_id]['stockprop_id'] = $property->stockprop_id;
          $this->items[$property->stock_id][$property->stockprop_id]['type_id'] = $property->type_id;
          $this->items[$property->stock_id][$property->stockprop_id]['type_name'] = check_plain($property->cvterm_name);
          $this->items[$property->stock_id][$property->stockprop_id]['value'] = check_plain($property->value);
          $this->items[$property->stock_id][$property->stockprop_id]['rank'] = $property->rank;

          $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$rk]['stock_id'] = $property->stock_id;
          $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$rk]['stockprop_id'] = $property->stockprop_id;
          $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$rk]['type_id'] = $property->type_id;
          $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$rk]['type_name'] = check_plain($property->cvterm_name);
          $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$rk]['value'] = check_plain($property->value);
          $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$rk]['rank'] = $property->rank;
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
      $items = $values->{$this->aliases['properties']}[$this->aliases['field_handler']];
      if (empty($items)) { $items = array(); }
      $rendered_items = array();
      foreach ($items as $k => $item) {
        if (!is_int($k)) {
          continue;
        }
        $text = array();
        if ($this->options['stockprop_display_options']['value']) {
          if ($item['value']) {
            $text[] =  $item['value'];
          } else {
            //display checkmark image
            $image_path = drupal_get_path('module','tripal_core').'/images/checkmark.gif';
            $text[] = '<img border="0" src="'.url($image_path).'" alt="checkmark" />';
          }
        }
        if ($this->options['stockprop_display_options']['type']) {
          $text[] = $item['cvterm_name'];
        }

        if (sizeof($text) > 1) {
          $rendered_items[] = $text[0].' ('.$text[1].')';
        } else {
          $rendered_items[] =  $text[0];
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
      if (empty($values->{$this->field_alias})) {
        $values->{$this->field_alias} = '';
      }
      return $this->parent_render($values);
    }
  }

  function document_self_tokens(&$tokens) {
    $tokens['[' . $this->options['id'] . '-stock_id' . ']'] = t('The Stock ID.');
    $tokens['[' . $this->options['id'] . '-stockprop_id' . ']'] = t('The Property ID.');
    $tokens['[' . $this->options['id'] . '-type_id' . ']'] = t('The Property Type ID.');
    $tokens['[' . $this->options['id'] . '-type_name' . ']'] = t('The Property Type.');
    $tokens['[' . $this->options['id'] . '-value' . ']'] = t('The Value of the Property.');
    $tokens['[' . $this->options['id'] . '-rank' . ']'] = t('The Rank of the Property.');
  }

  function add_self_tokens(&$tokens, $item) {
    $tokens['[' . $this->options['id'] . '-stock_id' . ']'] = $item['stock_id'];
    $tokens['[' . $this->options['id'] . '-stockprop_id' . ']'] = $item['stockprop_id'];
    $tokens['[' . $this->options['id'] . '-type_id' . ']'] = $item['type_id'];
    $tokens['[' . $this->options['id'] . '-type_name' . ']'] = $item['type_name'];
    $tokens['[' . $this->options['id'] . '-value' . ']'] = $item['value'];
    $tokens['[' . $this->options['id'] . '-rank' . ']'] = $item['rank'];
  }

}

