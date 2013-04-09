<?php

class views_handler_filter_crossingblock extends views_handler_filter {

  function init(&$view, $options) {
    parent::init(&$view, $options);

		// get teh stock property types cv set in Tripal -> Stock -> Configure
		$this->stock_prop_types_cv_id = variable_get('chado_stock_prop_types_cv', '');
    if ($this->stock_prop_types_cv_id) {

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

    }
  }

  // The exposed form available to the user if the administrator wills it
  function value_form(&$form, &$form_state) {

    $form['value'] = array(
      '#type' => 'crossing_block',
      '#default_value' => array(
      	'year' => (isset($form_state['input']['cb_year'])) ? $form_state['input']['cb_year'] : $this->options['cb_year'],
      	'season' => (isset($form_state['input']['cb_season'])) ? $form_state['input']['cb_season'] : $this->options['cb_season']
      ),
    );

  }


  // How the query should be modified based on the options selected
  function query () {

    if (!$this->broken) {

      if ($this->value[0]['year']) {
        $new_where_sql = "stock.stock_id IN (SELECT stockprop.stock_id FROM stockprop WHERE stockprop.type_id=".$this->cb_year_type_id." AND stockprop.value='".$this->value[0]['year']."')";
        $this->query->add_where($this->options['group'], $new_where_sql);
      }
      if ($this->value[0]['season']) {
        $new_where_sql = "stock.stock_id IN (SELECT stockprop.stock_id FROM stockprop WHERE stockprop.type_id=".$this->cb_season_type_id." AND stockprop.value='".$this->value[0]['season']."')";
        $this->query->add_where($this->options['group'], $new_where_sql);
      }

    } //end of if not broken
  }


}