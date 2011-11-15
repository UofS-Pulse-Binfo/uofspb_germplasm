<?php

class views_handler_filter_crossingblock extends views_handler_filter {

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