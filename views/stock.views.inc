<?php

function germplasm_additions_stock_views_data() {
	$data = array();
	
	// Field: Display the Parent number of a stock
	$data['stock']['parent_number'] = array(
		'title' => t('Parent Number'),
		'help' => t('The Parent Number of an assigned parent for the current crossing block.'),
		'field' => array(
			'handler' => 'views_handler_field_parent_number'
		),
		'filter' => array(
			'handler' => 'views_handler_filter_parent_number'
		),
		'sort' => array(
			'handler' => 'views_handler_sort_parent_number',
		),
	);
	
	// Argument: Used to filter a view based on the crossing block year in the path
	$data['stock']['pnum_cb_year'] = array(
		'title' => t('Parent Number: Crossing Block Year'),
		'help' => t('Sets the crossing block year a stock is the parent of'),
		'argument' => array(
			'handler' => 'views_handler_argument_no_query',
		),
	);

	// Argument: Used to filter a view based on the crossing block season in the path
	$data['stock']['pnum_cb_season'] = array(
		'title' => t('Parent Number: Crossing Block Season'),
		'help' => t('Sets the crossing block season a stock is the parent of'),
		'argument' => array(
			'handler' => 'views_handler_argument_no_query',
		),
	);
	
	$data['stock']['crossing_block'] = array(
	  'title' => t('Crossing Block'),
	  'help' => 'The crossing block a given stock was marked as progeny in',
	  'field' => array(
	    'handler' => 'views_handler_field_crossing_block',
	  ),
	  'filter' => array(
	  	'handler' => 'views_handler_filter_crossingblock',
	  ),
	  'sort' => array(
	    'handler' => 'views_handler_sort',
	  ),
	);
	
	$data['stock']['cross_number'] = array(
	  'title' => t('Cross Number'),
	  'help' => 'The cross number a cross was registered with',
	  'field' => array(
	    'handler' => 'views_handler_field_cross_number',
	  ),
	  'filter' => array(
	    'handler' => 'views_handler_filter_cross_number',
	  ),
	  'sort' => array(
	    'handler' => 'views_handler_sort_cross_number',
	  ),
	);	
	
	$data['stock']['is_germplasm'] = array(
		'title' => t('Is Germplasm?'),
		'help' => t('A boolean to indicate whether a given stock is considered germplasm or not. A stock is considered germplasm if it\'s main database reference is for the database set as the germplasm database.'),
		'filter'=> array(
			'handler' => 'views_handler_filter_is_germplasm',
		),
	);
	
	return $data;
}