<?php

class views_handler_filter_parent_number extends views_handler_filter {
	
  function init(&$view, $options) {
    parent::init(&$view, $options);
    
    $display_id = 'default';
		
		// get the stock property types cv
		$this->stock_prop_types_cv_id = variable_get('chado_stock_prop_types_cv', '');
		
    if ($this->stock_prop_types_cv_id) {
    
      // synonym cvterm_id
      $synonym_cvterm = tripal_cv_get_cvterm_by_name('synonym', $this->stock_prop_types_cv_id);
		  $this->synonym_type_id = $synonym_cvterm->cvterm_id;
		  if (empty($this->synonym_type_id)) {
        $cv = tripal_core_chado_select('cv',array('name'),array('cv_id'=>$this->stock_prop_types_cv_id));
        $cv_name = $cv[0]->name;
        $options = array();
		    drupal_set_message('Missing controlled vocabulary term <b>synonym</b> in stock property type controlled vocabulary ('.$cv_name.')', 'error');  		  
        $this->broken = TRUE;
		  }

		  //crossingblock_year cvterm_id
      $cb_year_cvterm = tripal_cv_get_cvterm_by_name('crossingblock_year', $this->stock_prop_types_cv_id);
	  	$this->cb_year_type_id = $cb_year_cvterm->cvterm_id;
		  if (empty($this->cb_year_type_id)) {
        $cv = tripal_core_chado_select('cv',array('name'),array('cv_id'=>$this->stock_prop_types_cv_id));
        $cv_name = $cv[0]->name;
		    drupal_set_message('Missing controlled vocabulary term <b>crossingblock_year</b> in stock property type controlled vocabulary ('.$cv_name.')', 'error');
        $this->broken = TRUE;
		  }		  

      // crossingblock_season cvterm_id
      $cb_season_cvterm = tripal_cv_get_cvterm_by_name('crossingblock_season', $this->stock_prop_types_cv_id);
		  $this->cb_season_type_id = $cb_season_cvterm->cvterm_id;
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
      drupal_set_message('Please set the stock type controlled vocabulary in Tripal Management -> Stock -> Configure', 'error');
      $this->broken = TRUE;    
    }

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
    
    if ($this->cb_year_type_id) {
      $previous_db = db_set_active('chado');
      $resource = db_query("SELECT value FROM stockprop WHERE type_id=".$this->cb_year_type_id." ORDER BY value");
      db_set_active($previous_db);
      $options = array('<NONE>'=>'None');
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
    	'#required' => TRUE,
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
      $options = array('<NONE>'=>'None');
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
    	'#required' => TRUE,
    );
    $form['use_cbseason_arg'] = array(
    	'#type' => 'checkbox',
    	'#title' => 'Set Season based on "Parent Number:Crossing Block Season Argument"',
    	'#default_value' => $this->options['use_cbseason_arg'],
    );
    
  }
  
  function query () {

	  if (!$this->broken) { 
      // set the crossing block from arguements
      if ($this->options['use_cbseason_arg']) {
        $season_arguement = $this->view->args[$this->view->argument['pnum_cb_season']->position];
        $this->options['cb_season'] = $season_arguement;
      }
      if ($this->options['use_cbyear_arg']) {
        $year_arguement = $this->view->args[$this->view->argument['pnum_cb_year']->position];
        $this->options['cb_year'] = $year_arguement;
      }
      
      //Determine synonym Prefix
      $synonym_prefix = '';
      if (preg_match('/[Ss]ummer/', $this->options['cb_season'])) { 
        $synonym_prefix = 'Sm'.$this->options['cb_year'].'-P'; 
      } elseif (preg_match('/[Ff]all/', $this->options['cb_season'])) { 
        $synonym_prefix = 'F'.$this->options['cb_year'].'-P'; 
      } elseif (preg_match('/[Ww]inter/', $this->options['cb_season'])) { 
        $synonym_prefix = 'W'.$this->options['cb_year'].'-P'; 
      } elseif (preg_match('/[Ss]pring/', $this->options['cb_season'])) { 
        $synonym_prefix = 'Sp'.$this->options['cb_year'].'-P'; 
      }
  
      $synonym_cvterm = tripal_cv_get_cvterm_by_name('synonym', variable_get('chado_stock_prop_types_cv', null));
      $synonym_type_id = $synonym_cvterm->cvterm_id;	
  
      $new_where_sql = "stock.stock_id IN (SELECT stockprop.stock_id FROM stockprop WHERE stockprop.type_id=".$synonym_type_id." AND stockprop.value~'".$synonym_prefix."')";
      $this->query->add_where($this->options['group'], $new_where_sql);
		} //end of if not broken
	}

}