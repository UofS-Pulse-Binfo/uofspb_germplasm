<?php

class views_handler_field_cross_number extends views_handler_field_stock_dbxref_by_type {

  function init(&$view, $options) {
    parent::init(&$view, $options);
    
    $this->options['display_options'][0] = 'accession';
    
    foreach (array_keys(variable_get( 'germplasm_cross_dbs', array() )) as $db_id) {
      $this->options['db_ids'][$db_id] = $db_id;
    }
    
  }
  
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['db_ids']['#disabled'] = TRUE;
  }
}