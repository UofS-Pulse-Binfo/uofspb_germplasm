<?php

class views_handler_sort_cross_number extends views_handler_sort {

  function init(&$view, $options) {
    parent::init(&$view, $options);

    $this->germplasm_cross_dbs = variable_get( 'germplasm_cross_dbs', array() );
    foreach (array_keys($this->germplasm_cross_dbs) as $db_id) {
      $this->options['db'][$db_id] = $db_id;
    }
    
  }
  
  function options_form (&$form, &$form_state) {
  
    $db_options = $this->germplasm_cross_dbs;
    $form['db'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Cross Databases'),
      '#description' => 'To sort cross numbers in a given cross database simply check only it below, '
        .'when selecting multiple cross datbases, if more then one cross number was assigned to a given '
        .'germplasm then the first one enetered will be used for sorting purposes.',
      '#options' => $db_options,
      '#default_value' => $this->options['db'],
    );
    
  }
  
  function query () {
  
    // Add OrderBY
    $this->query->add_orderby("sort","cross_number", $this->options['order']);

    // Change the field added by the orderby
    // the following substring regex extracts only the parent number and converts it to an integer
    // this allows true numerical sort or the parent number
    $this->query->fields['sort_cross_number']['table'] = NULL;
    $this->query->fields['sort_cross_number']['field'] = "(SELECT dbxref.accession FROM dbxref WHERE dbxref.dbxref_id IN (SELECT stock_dbxref.dbxref_id FROM stock_dbxref WHERE stock_dbxref.stock_id=stock.stock_id) AND dbxref.db_id IN (".implode(',',array_keys($this->germplasm_cross_dbs))."))";

  }
  
}