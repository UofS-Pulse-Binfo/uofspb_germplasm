<?php

class views_handler_filter_cross_number extends views_handler_filter_stock_dbxref_id {

  function init(&$view, $options) {
    parent::init($view, $options);
    
    foreach (array_keys(variable_get( 'germplasm_cross_dbs', array() )) as $db_id) {
      $this->options['db'][$db_id] = $db_id;
    }
    
    $this->db = $this->options['db'];
  }
  
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    
    $form['db']['#default_value'] = $this->options['db'];
    $form['db']['#disabled'] = TRUE;
    
  }
  
}