<?php

function germplasm_additions_stockprop_views_data() {
	$data = array();
	
	$data['stockprop']['table'] = array(
    'field' => 'stockprop_id',
    'database' => 'chado'
  );

  // Define relationships between this table and others
  $data['stockprop']['table']['join'] = array(
     'stock' => array(
       'left_field' => 'stock_id',
       'field' => 'stock_id',
       'type' => 'left outer',
     ),
  );
	
	return $data;
}