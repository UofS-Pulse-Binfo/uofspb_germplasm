<?php

class views_handler_field_crossing_block extends views_handler_field_stockprop_by_type {

  function init(&$view, $options) {
    parent::init(&$view, $options);

    // get the stock property types cv set in Tripal -> Stock -> Configure
		$this->stock_prop_types_cv_id = variable_get('chado_stock_prop_types_cv', '');
    if ($this->stock_prop_types_cv_id) {
		  //crossingblock_year cvterm_id
      $cb_year_cvterm = tripal_core_chado_select(
              'cvterm',
              array('cvterm_id'),
              array('name' => 'crossingblock_year',
                  'cv_id' => $this->stock_prop_types_cv_id
              ));
	  	$this->cb_year_type_id = $cb_year_cvterm[0]->cvterm_id;
		  if (empty($this->cb_year_type_id)) {
        $cv = tripal_core_chado_select('cv',array('name'),array('cv_id'=>$this->stock_prop_types_cv_id));
        $cv_name = $cv[0]->name;
		    drupal_set_message('Missing controlled vocabulary term <b>crossingblock_year</b> in stock property type controlled vocabulary ('.$cv_name.')', 'error');
        $this->broken = TRUE;
		  }

      // crossingblock_season cvterm_id
      $cb_season_cvterm = tripal_core_chado_select(
              'cvterm',
              array('cvterm_id'),
              array('name' => 'crossingblock_season',
                  'cv_id' => $this->stock_prop_types_cv_id));
		  $this->cb_season_type_id = $cb_season_cvterm[0]->cvterm_id;
		  if (empty($this->cb_season_type_id)) {
        $cv = tripal_core_chado_select('cv',array('name'),array('cv_id'=>$this->stock_prop_types_cv_id));
        $cv_name = $cv[0]->name;
		    drupal_set_message('Missing controlled vocabulary term <b>crossingblock_season</b> in stock property type controlled vocabulary ('.$cv_name.')', 'error');
        $this->broken = TRUE;
		  }
    } else {
      drupal_set_message('Please set the stock property type controlled vocabulary in Tripal Management -> Stock -> Configure', 'error');
      $this->broken = TRUE;
    }

    // Set the crossingblock year and season stcok properties to true
    $this->options['stockprop_type_ids'] = array(
      $this->cb_year_type_id => $this->cb_year_type_id,
      $this->cb_season_type_id => $this->cb_season_type_id,
    );

  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_crossingblock'] = array('default' => TRUE);
    $options['cb_display_options'] = array('default' => array('year', 'season','year'=>'year','season'=>'season'));
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['stockprop_type_ids']['#disabled'] = TRUE;
    unset($form['stockprop_display_options']);

    $form['cb_display_options'] = array(
      '#prefix' => '<div><div id="edit-options-display_options">',
      '#suffix' => '</div></div>',
      '#type' => 'checkboxes',
      '#title' => t('Display Crossing Block Parts'),
      '#description' => t('Check each part of the crossing block you want displayed'),
      '#options' => array(
        'year' => 'Year',
        'season' => 'Season',
      ),
      '#default_value' => array($this->options['cb_display_options']['year'], $this->options['cb_display_options']['season']),
    );

    $form['link_to_crossingblock'] = array(
      '#type' => 'checkbox',
      '#title' => 'Link each crossing block to its progeny list',
      '#default_value' => $this->options['link_to_crossingblock'],
    );
  }

  function pre_render(&$values) {
    $this->aliases['properties'] = 'stockprop_all';
    $this->aliases['stock_id'] = 'stock_id';
    $this->field_alias = $this->aliases['properties'];
    $this->aliases['field_handler'] = 'crossingblock_' . $this->position;

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
    foreach ($values as $vk => $result) {
      if (!empty($result->{$this->aliases['properties']})) {

        // all properties for the current stock
        $properties = $result->{$this->aliases['properties']};
        foreach ($properties as $rk => $property) {
          // perform filtering
          $property = (object) $property;
          if (!empty($types2keep)) {
            if (!in_array($property->type_id, $types2keep)) {
              continue;
            }
          }

          $organism_common_name = preg_replace('/\s+/','-',$result->organism_common_name);
          $this->items[$property->stock_id][$property->rank]['organism_common_name'] = $organism_common_name;
          if (strcmp('crossingblock_year',$property->type_name) == 0) {
            // Add property to the list of items to be rendered
            $this->items[$property->stock_id][$property->rank]['crossingblock_year'] = check_plain($property->value);
            $this->items[$property->stock_id][$property->rank]['crossingblock_year_stockprop_id'] = $property->stockprop_id;
          } elseif (strcmp('crossingblock_season',$property->type_name) == 0) {
            // Add property to the list of items to be rendered
            $this->items[$property->stock_id][$property->rank]['crossingblock_season'] = check_plain($property->value);
            $this->items[$property->stock_id][$property->rank]['crossingblock_season_stockprop_id'] = $property->stockprop_id;
          }

          $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$property->rank]['organism_common_name'] = $organism_common_name;
          if ($property->type_id == $this->cb_year_type_id) {
            // Add property to the list of items to be rendered
            $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$property->rank]['crossingblock_year'] = check_plain($property->value);
            $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$property->rank]['crossingblock_year_stockprop_id'] = $property->stockprop_id;
          } elseif ($property->type_id == $this->cb_season_type_id) {
            // Add property to the list of items to be rendered
            $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$property->rank]['crossingblock_season'] = check_plain($property->value);
            $values[$vk]->{$this->aliases['properties']}[$this->aliases['field_handler']][$property->rank]['crossingblock_season_stockprop_id'] = $property->stockprop_id;
          }
        } //end of foreach property
      } //end of if properties
    } //end of foreach values
  } //end of pre_render function

  function render($values) {

    $items = $values->{$this->aliases['properties']}[$this->aliases['field_handler']];
    if (empty($items)) { $items = array(); }
    $rendered_items = array();
    foreach ($items as $item) {
      $text = array();
      if ($this->options['cb_display_options']['season']) {
        $text[] = $item['crossingblock_season'];
      }

      if ($this->options['cb_display_options']['year']) {
        $text[] = $item['crossingblock_year'];
      }

      if ($this->options['link_to_crossingblock']) {
        $link = 'Crossing_Block/'.$item['organism_common_name'].'/'.$item['crossingblock_year'].'/'.$item['crossingblock_season'].'/Progeny';
        $rendered_items[] = l(implode(' ',$text),$link);
      }
      else {
        $rendered_items[] = implode(' ',$text);
      }
    }

    if ($this->options['type'] == 'separator') {
      return implode(check_plain($this->options['separator']), $rendered_items);
    }
    else {
      return theme('item_list', $rendered_items, NULL, $this->options['type']);
    }
  }

}