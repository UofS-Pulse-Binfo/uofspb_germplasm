<?php

class views_handler_field_parent_number extends views_handler_field_prerender_list {

  function init(&$view, $options) {
    parent::init(&$view, $options);

		$display_id = 'default';

		// get teh stock property types cv set in Tripal -> Stock -> Configure
		$this->stock_prop_types_cv_id = variable_get('chado_stock_prop_types_cv', '');
    if ($this->stock_prop_types_cv_id) {
      // synonym cvterm_id
      $synonym_cvterm = tripal_core_chado_select(
        'cvterm',
        array('cvterm_id'),
        array(
          'name' => 'synonym',
          'cv_id' => $this->stock_prop_types_cv_id
        )
      );
		  $this->synonym_type_id = $synonym_cvterm[0]->cvterm_id;
		  if (empty($this->synonym_type_id)) {
        $cv = tripal_core_chado_select('cv',array('name'),array('cv_id'=>$this->stock_prop_types_cv_id));
        $cv_name = $cv[0]->name;
        $options = array();
		    drupal_set_message('Missing controlled vocabulary term <b>synonym</b> in stock property type controlled vocabulary ('.$cv_name.')', 'error');
		  }

		  //crossingblock_year cvterm_id
      $cb_year_cvterm = tripal_core_chado_select(
        'cvterm',
        array('cvterm_id'),
        array(
          'name' => 'crossingblock_year',
          'cv_id' => $this->stock_prop_types_cv_id
        )
      );
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
        array(
          'name' => 'crossingblock_season',
          'cv_id' => $this->stock_prop_types_cv_id
        )
      );
		  $this->cb_season_type_id = $cb_season_cvterm[0]->cvterm_id;
		  if (empty($this->cb_season_type_id)) {
        $cv = tripal_core_chado_select('cv',array('name'),array('cv_id'=>$this->stock_prop_types_cv_id));
        $cv_name = $cv[0]->name;
		    drupal_set_message('Missing controlled vocabulary term <b>crossingblock_season</b> in stock property type controlled vocabulary ('.$cv_name.')', 'error');
        $this->broken = TRUE;
		  }

      // Check if there is currently a property field
      // This is needed because the parent number will be extracted from a synonym (property)
      $properties_field_exists = FALSE;
      foreach ($view->display[$display_id]->display_options['fields'] as $name => $field) {
        if (preg_match('/properties/', $name)) {
          if (!empty($field['stockprop_type_ids'][$this->synonym_type_id])) {
            $properties_field_exists = TRUE;
          }
        }
      }

      // if property field doesn't already exist then add one
      if (!$properties_field_exists) {
        $id = $view->add_item($display_id, 'field', 'stock', 'properties',
                              array('exclude'=>TRUE,
                                    'stockprop_type_ids'=>array(
                                      $this->synonym_type_id => $this->synonym_type_id
                                     ),
                                     'label' => 'Synonyms'
                              ));
      }
    } else {
      drupal_set_message('Please set the stock property type controlled vocabulary in Tripal Management -> Stock -> Configure', 'error');
      $this->broken = TRUE;
    }

  }

  function option_definition() {
    $options = parent::option_definition();
    $options['display_options'] = array(
      'parent_number'=>'parent_number',
      'crossing_block'=>'crossing_block',
		);
		return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    if ($this->broken) {
      $form = array(
        'msg' => array(
          '#type' => 'item',
          '#value' => 'This field is currently broken. Please see any error messages that may have come up '
            .'for further instructions on how to fix this',
        ),
      );
      return $form;
    }

		$form['display_options'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Display Parent Number &/or Crossing Block'),
			'#options' => array(
				'parent_number' => 'Parent Number',
				'crossing_block' => 'Crossing Block',
			),
			'#default_value' => $this->options['display_options'],
		);

    if ($this->cb_year_type_id) {
      $previous_db = tripal_db_set_active('chado');
      $resource = db_query("SELECT value FROM stockprop WHERE type_id=".$this->cb_year_type_id." ORDER BY value");
      tripal_db_set_active($previous_db);
      $options = array();
      $options[0] = 'Select the Crossing Block Year';
      while ($r = db_fetch_object($resource)) {
        $options[$r->value] = $r->value;
      }
		} else {
		  $options = array('<NONE>'=>'None');
		}
    $form['cb_year'] = array(
    	'#type' => 'select',
    	'#title' => t('Crossing Block Year'),
    	'#options' => $options,
			'#description' => 'If Selected the only parent numbers from the selected year are displayed.',
			'#default_value' => $this->options['cb_year'],
    );

    $form['use_cbyear_arg'] = array(
    	'#type' => 'checkbox',
    	'#title' => 'Set Year based on "Parent Number:Crossing Block Year Argument"',
    	'#default_value' => $this->options['use_cbyear_arg'],
    );

    if ($this->cb_season_type_id) {
      $previous_db = db_set_active('chado');
      $resource = db_query("SELECT value FROM stockprop WHERE type_id=".$this->cb_season_type_id." ORDER BY value");
      db_set_active($previous_db);
      $options = array();
      $options[0] = 'Select the Crossing Block Season';
      while ($r = db_fetch_object($resource)) {
        $options[$r->value] = $r->value;
      }
		} else {
		  $options = array('<NONE>'=>'None');
    }
    $form['cb_season'] = array(
    	'#type' => 'select',
    	'#title' => t('Crossing Block Season'),
    	'#options' => $options,
    	'#description' => 'If Selected the only parent numbers from the selected season are displayed.',
    	'#default_value' => $this->options['cb_season'],
    );

    $form['use_cbseason_arg'] = array(
    	'#type' => 'checkbox',
    	'#title' => 'Set Season based on "Parent Number:Crossing Block Season Argument"',
    	'#default_value' => $this->options['use_cbseason_arg'],
    );

    $form['link_to_cb_parent_view'] = array(
      '#type' => 'checkbox',
      '#title' => 'Link to Crossing Block Parent View',
    );
	}

  function query () {
    if ($this->broken) { return false; }
   //do nothing
  }

	function pre_render($values) {
	  if ($this->broken) { return false; }

		$this->aliases['properties'] = 'properties';
		$this->aliases['stock_id'] = 'stock_id';
		$this->field_alias = $this->aliases['stock_id'];

		// set the crossing block from arguements
		if ($this->options['use_cbseason_arg']) {
			$season_arguement = $this->view->args[$this->view->argument['pnum_cb_season']->position];
			$this->options['cb_season'] = $season_arguement;
		}
		if ($this->options['use_cbyear_arg']) {
			$year_arguement = $this->view->args[$this->view->argument['pnum_cb_year']->position];
			$this->options['cb_year'] = $year_arguement;
		}

		//for each stock in this view page
		foreach ($values as $result) {
			if (!empty($result->{$this->aliases['properties']})) {

				// all properties for the current stock
				$properties = $result->{$this->aliases['properties']};
				foreach ($properties as $property) {
					// perform filtering
					if ($property->type_id == $this->synonym_type_id) {
						$synonym = check_plain($property->value);
						if (preg_match('/(Sm|F|W|Sp)(\d+)-P(\d+)/',$synonym, $matches)) {

							// Transform the Season to the full word
							if ($matches[1] == 'Sm') {
								$season = 'Summer';
							} elseif ($matches[1] == 'F') {
								$season = 'Fall';
							} elseif ($matches[1] == 'W') {
								$season = 'Winter';
							} elseif ($matches[1] == 'Sp') {
								$season = 'Spring';
							} else {
								$season = $matches[1];
							}

							$year = $matches[2];
							$pnumber = $matches[3];

							// Do filtering if these options were supplied
							if (!empty($this->options['cb_year'])) {
								if ($this->options['cb_year'] != $year) { continue; }
							}
							if (!empty($this->options['cb_season'])) {
								if (!preg_match('/'.$season.'/i',$this->options['cb_season'])) { continue; }
							}

							$this->items[$property->stock_id][$property->stockprop_id]['crossingblock_season'] = $season;
							$this->items[$property->stock_id][$property->stockprop_id]['crossingblock_year'] = $year;
							$this->items[$property->stock_id][$property->stockprop_id]['parent_number'] = $pnumber;
							$organism_common_name = preg_replace('/\s+/','-',$result->organism_common_name);
							$this->items[$property->stock_id][$property->stockprop_id]['cb_link'] = 'Crossing_Block/'.$organism_common_name.'/'.$year.'/'.$season.'/Parents';
						}
					}
				}	//end of foreach property
			} //end of if there are properties
		} //end of foreach result

	}

	function render_item($count, $item) {
	  if ($this->broken) { return false; }

		$text = array();

		if ($this->options['display_options']['parent_number']) {
			$text[] = $item['parent_number'];
		}
		if ($this->options['display_options']['crossing_block']) {
			$text[] = $item['crossingblock_season'].' '.$item['crossingblock_year'];
		}

		if (sizeof($text) == 1) {
			$name = $text[0];
		} elseif (sizeof($text) == 2) {
			$name = $text[0] .' ('.$text[1].')';
		}

		if ($this->options['link_to_cb_parent_view']) {
		  return l($name, $item['cb_link']);
		} else {
		  return $name;
		}
	}
}