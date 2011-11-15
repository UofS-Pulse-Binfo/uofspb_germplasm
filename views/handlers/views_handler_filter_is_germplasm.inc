<?php

class views_handler_filter_is_germplasm extends views_handler_filter {

  function init(&$view, $options) {
    parent::init(&$view, $options);
    
    // Add main germplasm db as set in germplasm configuration: Germplasm Prefix->Database
    $this->germplasm_db_id = variable_get('tripal_stock_main_dbxref_db', 0);
    if (!empty($this->germplasm_db_id)) {
      $db = tripal_core_chado_select('db',array('name'),array('db_id'=>$this->germplasm_db_id)); 
      $this->germplasm_db_name = $db[0]->name;
    }
    
    $this->value = $this->germplasm_db_name;
    
  }
  
 /**
  * Add to options form description telling the user what's going on
  * Namely that a filter will be applied that shows only stocks with 
  * main dbxrefs with dbs in this list
  */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
   
    $form['msg'] = array(
      '#type' => 'item',
      '#value' => 'This filter will cause only stocks with a main database reference in the '
        .$this->germplasm_db_name.' database to be displayed. Since '.$this->germplasm_db_name.' is the '
        .'database set to be the germplasm prefix database, this will be a list of all germplasm.'
    );
  }

 /**
  * Alters the query to ensure that all stocks with dbxrefs 
  * where dbxref.db_id=$this->germplasm_db_id
  */
  function query () {
    $new_where_sql = "stock.dbxref_id IN (SELECT dbxref.dbxref_id FROM dbxref WHERE dbxref.db_id=".$this->germplasm_db_id.")";
    $this->query->add_where($this->options['group'], $new_where_sql);
  }
}