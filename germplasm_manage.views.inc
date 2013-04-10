<?php

/**
 *  @file
 *  This file contains extensions for views integration of chado/tripal stock
 *  tables with germplasm management in mind. Supplementary functions can be
 *  found in ./views/
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
 */

/**
 * Implements hook_views_data()
 * Purpose: Describe chado/tripal tables & fields to views
 *
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 */
function germplasm_manage_views_data()  {

  // Add Fields to stock integration
  if (!tripal_views_is_integrated('stock',6)) {
    $fields = array();

    // Field: Display the Parent number of a stock
    $fields['parent_number'] = array(
      'name' => 'parent_number',
      'title' => 'Parent Number',
      'description' => 'The Parent Number of an assigned parent for the current crossing block.',
      'type' => 'custom',
      'handlers' => array(
        'field' => array(
          'name' => 'views_handler_field_parent_number'
        ),
        'filter' => array(
          'name' => 'views_handler_filter_parent_number'
        ),
        'sort' => array(
          'name' => 'views_handler_sort_parent_number',
        ),
      ),
      'joins' => array(),
    );

    // Argument: Used to filter a view based on the crossing block year in the path
    $fields['pnum_cb_year'] = array(
      'name' => 'pnum_cb_year',
      'title' => 'Parent Number: Crossing Block Year',
      'description' => 'Sets the crossing block year a stock is the parent of',
      'type' => 'custom',
      'handlers' => array(
        'argument' => array(
          'name' => 'views_handler_argument_no_query',
        ),
      ),
      'joins' => array(),
    );

    // Argument: Used to filter a view based on the crossing block season in the path
    $fields['pnum_cb_season'] = array(
      'name' => 'pnum_cb_season',
      'title' => 'Parent Number: Crossing Block Season',
      'description' => 'Sets the crossing block season a stock is the parent of',
      'type' => 'custom',
      'handlers' => array(
        'argument' => array(
          'name' => 'views_handler_argument_no_query',
        ),
      ),
      'joins' => array(),
    );

    $fields['crossing_block'] = array(
      'name' => 'crossing_block',
      'title' => 'Crossing Block',
      'description' => 'The crossing block a given stock was marked as progeny in',
      'type' => 'custom',
      'handlers' => array(
        'field' => array(
          'name' => 'views_handler_field_crossing_block',
        ),
        'filter' => array(
          'name' => 'views_handler_filter_crossingblock',
        ),
        'sort' => array(
          'name' => 'views_handler_sort',
        ),
      ),
      'joins' => array(),
    );

    $fields['cross_number'] = array(
      'name' => 'cross_number',
      'title' => 'Cross Number',
      'description' => 'The cross number a cross was registered with',
      'type' => 'custom',
      'handlers' => array(
        'field' => array(
          'name' => 'views_handler_field_cross_number',
        ),
        'filter' => array(
          'name' => 'views_handler_filter_cross_number',
        ),
        'sort' => array(
          'name' => 'views_handler_sort_cross_number',
        ),
      ),
      'joins' => array(),
    );

    $fields['is_germplasm'] = array(
      'name' => 'is_germplasm',
      'title' => 'Is Germplasm?',
      'description' => 'A boolean to indicate whether a given stock is considered germplasm or not. A stock is considered germplasm if it\'s main database reference is for the database set as the germplasm database.',
      'type' => 'custom',
      'handlers' => array(
        'filter'=> array(
          'name' => 'views_handler_filter_is_germplasm',
        ),
      ),
      'joins' => array(),
    );

    // STOCK DEPRECATED ------
    //Calculated Field: stock properties
    // uses a custom field handler which pulls results from the view
    // actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
    $fields['properties'] = array(
      'name' => 'properties',
      'title' => 'Stock Properties',
      'description' => 'Properties of the current stock.',
      'type' => 'custom',
      'handlers' => array(
        'field' => array(
          'title' => 'Properties',
          'help' => 'Display a given type of properties associated with a stock.',
          'name' => 'views_handler_field_stockprop_by_type',
        ),
        'filter' => array(
          'title' => 'Properties',
          'help' => 'Filter by a given property type or value.',
          'name' => 'views_handler_filter_stockprop_id',
        ),
        'argument' => array(
          'title' => 'Properties',
          'help' => 'Allow a property to be supplied in the path',
          'name' => 'views_handler_argument_stockprop_id',
        ),
      ),
      'joins' => array(),
    );

    //Calculated Field: stock properties (ALL)
    // uses a custom field handler which pulls results from the view
    // actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
    $fields['all_properties'] = array(
      'name' => 'all_properties',
      'title' => 'All Stock Properties',
      'description' => 'Properties of the current stock.',
      'type' => 'custom',
      'handlers' => array(
        'field' => array(
          'title' => 'All Properties',
          'help' => 'Display all properties associated with a stock.',
          'name' => 'views_handler_field_stockprop_all',
        ),
      ),
      'joins' => array(),
    );

    //Calculated Field: stock relationships
    // uses a custom field handler which pulls results from the view
    // actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
    $fields['relationships'] = array(
      'name' => 'relationships',
      'title' => 'Stock Relationships',
      'description' => 'Relationships including the current stock.',
      'type' => 'custom',
      'handlers' => array(
        'field' => array(
          'title' => 'Relationships',
          'help' => 'Display a given type of relationships including the current stock.',
          'name' => 'views_handler_field_stockrel_by_type',
        ),
        'filter' => array(
          'name' => 'views_handler_filter_stock_relationship_id'
        ),
      ),
      'joins' => array(),
    );

    //Calculated Field: stock relationships (ALL)
    // uses a custom field handler which pulls results from the view
    // actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
    $fields['all_relationships'] = array(
      'name' => 'all_relationships',
      'title' => 'All Stock Relationships',
      'description' => 'Relationships including the current stock.',
      'type' => 'custom',
      'handlers' => array(
        'field' => array(
          'title' => 'All Relationships',
          'help' => 'Display all relationships including the stock.',
          'name' => 'views_handler_field_stockrel_all',
        ),
      ),
      'joins' => array(),
    );

    //Calculated Field: stock dbxrefs
    // uses a custom field handler which pulls results from the view
    // actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
    $fields['dbxref'] = array(
      'name' => 'dbxref',
      'title' => 'Stock Database References',
      'description' => 'Database References associated with the current stock.',
      'type' => 'custom',
      'handlers' => array(
        'field' => array(
          'title' => 'Database References',
          'help' => 'Display database references from a given database for the current stock.',
          'name' => 'views_handler_field_stock_dbxref_by_type',
        ),
        'filter' => array(
          'title' => 'Database References',
          'help' => 'Filter by a given database reference type and/or value.',
          'name' => 'views_handler_filter_stock_dbxref_id',
        ),
      ),
      'joins' => array(),
    );

    //Calculated Field: stock dbxrefs (ALL)
    // uses a custom field handler which pulls results from the view
    // actual query performed in chado_stock_views_views_pre_render	(&$view) -file:tripal_stock.views.inc
    $fields['all_dbxref'] = array(
      'name' => 'all_dbxref',
      'title' => 'All Stock Database References',
      'description' => 'Database References associated with the current stock.',
      'type' => 'custom',
      'handlers' => array(
        'field' => array(
          'title' => 'All Database References',
          'help' => 'Display all database references for the current stock.',
          'name' => 'views_handler_field_stock_dbxref_all',
        ),
      ),
      'joins' => array(),
    );

    $table = 'stock';
    $priority = 6;
    foreach ($fields as $field_array) {
      tripal_views_add_field_to_integration($table, $priority, $field_array);
    }
  }

}

/**
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field",
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return: An array of handler definitions
 */
function germplasm_manage_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'germplasm_manage') . '/views/handlers',
   ),
   'handlers' => array(
     //Deprecated stock handlers
     // Updated to work with Tripal 1.0
     'views_handler_field_stock_dbxref_all' => array(
       'parent' => 'tripal_views_handler_field_aggregate',
     ),
     'views_handler_field_stock_dbxref_by_type' => array(
       'parent' => 'tripal_views_handler_field_aggregate',
     ),
     'views_handler_field_stockprop_all' => array(
       'parent' => 'tripal_views_handler_field_aggregate',
     ),
     'views_handler_field_stockprop_by_type' => array(
       'parent' => 'tripal_views_handler_field_aggregate',
     ),
     'views_handler_field_stockrel_all' => array(
       'parent' => 'tripal_views_handler_field_aggregate',
     ),
     'views_handler_field_stockrel_by_type' => array(
       'parent' => 'tripal_views_handler_field_aggregate',
     ),
     'views_handler_filter_stockprop_id' => array(
       'parent' => 'views_handler_filter',
     ),
     'views_handler_filter_stock_relationship_id' => array(
       'parent' => 'views_handler_filter',
     ),
     'views_handler_argument_stockprop_id' => array(
       'parent' => 'views_handler_argument_string',
     ),
     //Deprecated stock handlers
     //Original; not updated for Tripal 1.0
     'views_handler_filter_stock_dbxref_id' => array(
       'parent' => 'views_handler_filter',
     ),

      // germplasm manage fields
      // all upgraded to work with Tripal 1.0
     'views_handler_field_parent_number' => array(
     	'parent' => 'views_handler_field_stockprop_by_type',
     ),
     'views_handler_field_crossing_block' => array(
       'parent' => 'views_handler_field_stockprop_by_type',
     ),
     'views_handler_field_cross_number' => array(
       'parent' => 'views_handler_field_stock_dbxref_by_type',
     ),

     // germplasm manage filters
     // all upgraded to work with Tripal 1.0
     'views_handler_filter_cross_number' => array(
       'parent' => 'views_handler_filter_stock_dbxref_id',
     ),
     'views_handler_filter_crossingblock' => array(
       'parent' => 'views_handler_filter',
     ),
     'views_handler_filter_is_germplasm' => array(
      'parent' => 'views_handler_filter',
     ),
     'views_handler_filter_parent_number' => array(
     	'parent' => 'views_handler_filter',
     ),

     // germplasm manage sort
     // all upgraded to work with Tripal 1.0
     'views_handler_sort_cross_number' => array(
       'parent' => 'views_handler_sort',
     ),
     'views_handler_sort_parent_number' => array(
     	'parent' => 'views_handler_sort',
     ),

     // germplasm manage arguements
     // this one is used for crossingblock year/season
     'views_handler_argument_no_query' => array(
      'parent' => 'views_handler_argument_string'
     ),
   ),
 );
}

/**
 * Descirbes all default views for this module
 */
function germplasm_manage_views_default_views() {
	$veiws = array();
  $vars = array();

  // set variables used when defining default views
  // db_ids selected to be cross number databases
  $vars['cross_num_db_options'] = array();
  foreach (variable_get('germplasm_cross_dbs', array()) as $k => $v) {
    $vars['cross_num_db_options'][$k] = $k;
  }

  //synonym cvterm
  $synonym = tripal_core_chado_select('cvterm',array('cvterm_id'), array('name'=>'synonym', 'cv_id'=>variable_get('chado_stock_prop_types_cv',NULL)));
  if ($synonym[0]->cvterm_id) {
    $vars['properties_synonym_options'] = array(
      $synonym[0]->cvterm_id => $synonym[0]->cvterm_id,
    );
  } else {
    $vars['properties_synonym_options'] = array();
    drupal_set_message('Unable to determine the synonym controlled vocabulary term ID when generating default Germplasm views. Ensure that the Tripal Stock property CV is set an contains the term synonym.','warning');
  }

  //crossingblock_year and crossingblock_season cvterms
  $cb_year = tripal_core_chado_select('cvterm',array('cvterm_id'), array('name'=>'crossingblock_year', 'cv_id'=>variable_get('chado_stock_prop_types_cv',NULL)));
  $cb_season = tripal_core_chado_select('cvterm',array('cvterm_id'), array('name'=>'crossingblock_season', 'cv_id'=>variable_get('chado_stock_prop_types_cv',NULL)));
  if ($cb_year[0]->cvterm_id && $cb_season[0]->cvterm_id) {
    $vars['properties_crossingblock_options'] = array(
      $cb_year[0]->cvterm_id => $cb_year[0]->cvterm_id,
      $cb_season[0]->cvterm_id => $cb_season[0]->cvterm_id
    );
  } else {
    $vars['properties_crossingblock_options'] = array();
    drupal_set_message('Unable to determine the crossingblock_season/year controlled vocabulary term IDs when generating default Germplasm views. Ensure that the Tripal Stock property CV is set an contains the terms crossingblock_season and crossingblock_year.','warning');
  }

  // is_maternal_parent_of cvterm
  $mom_cv = tripal_core_chado_select('cvterm',array('cvterm_id'), array('name'=>'is_maternal_parent_of', 'cv_id'=>variable_get('chado_stock_relationship_cv',NULL)));
  if ($mom_cv[0]->cvterm_id) {
    $vars['rels_maternal_options'] = array( $mom_cv[0]->cvterm_id => $mom_cv[0]->cvterm_id );
    $vars['rels_maternal_type_id'] = $mom_cv[0]->cvterm_id;
  } else {
    $vars['rels_maternal_options'] = array();
    $vars['rels_maternal_type_id'] = NULL;
    drupal_set_message('Unable to determine the is_maternal_parent_of controlled vocabulary term IDs when generating default Germplasm views. Ensure that the Tripal Stock relationship CV is set an contains the term is_maternal_parent_of.','warning');
  }

  // is_paternal_parent_of cvterm
  $dad_cv = tripal_core_chado_select('cvterm',array('cvterm_id'), array('name'=>'is_paternal_parent_of', 'cv_id'=>variable_get('chado_stock_relationship_cv',NULL)));
  if ($dad_cv[0]->cvterm_id) {
    $vars['rels_paternal_options'] = array( $dad_cv[0]->cvterm_id => $dad_cv[0]->cvterm_id );
    $vars['rels_paternal_type_id'] = $dad_cv[0]->cvterm_id;
  } else {
    $vars['rels_paternal_options'] = array();
    $vars['rels_paternal_type_id'] = NULL;
    drupal_set_message('Unable to determine the is_paternal_parent_of controlled vocabulary term IDs when generating default Germplasm views. Ensure that the Tripal Stock relationship CV is set an contains the term is_paternal_parent_of.','warning');
  }

	// Default View: All Germplasm
	// This view lists all germplasm (ie: all stocks with a main database reference as set in the germplasm configuration)
	$views['germplasm_all'] = germplasm_manage_germplasm_default_view($vars);

	// Default View: Progeny for a given Crossing Block
	// List all the progeny of a given crossing block; crossing block chosen in path
	$views['crossingblock_progeny'] = germplasm_manage_crossingblock_progeny_default_view($vars);

	// Default View: Parents for Crossing Block
	// List all the parents of a given crossing block; crossing block chosen in path
	$views['crossingblock_parents'] = germplasm_manage_crossingblock_parents_default_view($vars);

	// Default View: Cross assigned parents
	// This view is used exclusively in the add/cross_parents form to show the user what parents have already been assigned
	$views['cross_assigned_parents'] = germplasm_manage_cross_assigned_parents_default_view($vars);

	return $views;
}

/**
 * Default View: All Germplasm
 * This view lists all germplasm (ie: all stocks with a main database reference as set in the germplasm configuration)
 *
 * @param $vars
 *   An array of variables for the default view to use including
 *     - cross_num_db_options
 *     - properties_synonym_options
 *     - properties_crossingblock_options
 *     - rels_maternal_options
 *     - rels_paternal_options
 *     - rels_maternal_type_id
 *     - rels_paternal_type_id
 * @return
 *   A default view object
 */
function germplasm_manage_germplasm_default_view($vars) {

  $view = new view;
  $view->name = 'germplasm_all';
  $view->description = 'Generic search/browse for germplasm.';
  $view->tag = 'chado default';
  $view->base_table = 'stock';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'nid' => array(
      'label' => 'Stock to Node',
      'required' => 0,
      'id' => 'nid',
      'table' => 'chado_stock',
      'field' => 'nid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('fields', array(
    'nid' => array(
      'label' => 'Nid',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'link_to_node' => 0,
      'exclude' => 1,
      'id' => 'nid',
      'table' => 'node',
      'field' => 'nid',
      'relationship' => 'none',
    ),
    'uniquename' => array(
      'label' => 'Unique Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'uniquename',
      'table' => 'stock',
      'field' => 'uniquename',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'stock',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'cross_number' => array(
      'label' => 'Cross Number',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'stockprop_type_ids' => array(),
      'display_options' => array(
        'accession' => 'accession',
        'link' => 'link',
        'db_name' => 0,
      ),
      'exclude' => 0,
      'id' => 'cross_number',
      'table' => 'stock',
      'field' => 'cross_number',
      'db_ids' => $vars['cross_num_db_options'],
      'relationship' => 'none',
    ),
    'properties' => array(
      'label' => 'Synonyms',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'stockprop_type_ids' => $vars['properties_synonym_options'],
      'stockprop_display_options' => array(
        'value' => 'value',
        'type' => 0,
      ),
      'exclude' => 0,
      'id' => 'properties',
      'table' => 'stock',
      'field' => 'properties',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'label' => 'Type',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'name_1',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'common_name' => array(
      'label' => 'Organism',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'relationship' => 'none',
    ),
    'crossing_block' => array(
      'label' => 'Crossing Block',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'stockprop_type_ids' => $vars['properties_crossingblock_options'],
      'stockprop_display_options' => array(
        0 => 'type',
        1 => 'value',
      ),
      'link_to_crossingblock' => 1,
      'cb_display_options' => array(
        'year' => 'year',
        'season' => 'season',
      ),
      'exclude' => 0,
      'id' => 'crossing_block',
      'table' => 'stock',
      'field' => 'crossing_block',
      'relationship' => 'none',
    ),
    'parent_number' => array(
      'label' => 'Parent In Crossing Block',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'stockprop_type_ids' => array(),
      'stockprop_display_options' => array(
        0 => 'type',
        1 => 'value',
      ),
      'display_options' => array(
        'crossing_block' => 'crossing_block',
        'parent_number' => 0,
      ),
      'exclude' => 0,
      'id' => 'parent_number',
      'table' => 'stock',
      'field' => 'parent_number',
      'relationship' => 'none',
      'cb_year' => '0',
      'use_cbyear_arg' => 0,
      'cb_season' => '0',
      'use_cbseason_arg' => 0,
      'link_to_cb_parent_view' => 1,
    ),
    'relationships' => array(
      'label' => 'Maternal Parent',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'stockrel_type_ids' => $vars['rels_maternal_options'],
      'stockrel_display_options' => array(
        0 => 'subject',
        1 => 'type',
        2 => 'object',
      ),
      'exclude' => 0,
      'id' => 'relationships',
      'table' => 'stock',
      'field' => 'relationships',
      'relationship' => 'none',
      'stockrel_display_parts' => array(
        'subject' => 'subject',
        'type' => 0,
        'object' => 0,
      ),
      'stockrel_display_rels' => 'object',
    ),
    'relationships_1' => array(
      'label' => 'Paternal Parent',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'stockrel_type_ids' => $vars['rels_paternal_options'],
      'stockrel_display_options' => array(
        0 => 'subject',
        1 => 'type',
        2 => 'object',
      ),
      'exclude' => 0,
      'id' => 'relationships_1',
      'table' => 'stock',
      'field' => 'relationships',
      'relationship' => 'none',
      'stockrel_display_parts' => array(
        'subject' => 'subject',
        'type' => 0,
        'object' => 0,
      ),
      'stockrel_display_rels' => 'object',
    ),
    'all_dbxref' => array(
      'label' => 'All Database References',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_dbxref',
      'table' => 'stock',
      'field' => 'all_dbxref',
      'relationship' => 'none',
    ),
    'all_properties' => array(
      'label' => 'All Properties',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_properties',
      'table' => 'stock',
      'field' => 'all_properties',
      'relationship' => 'none',
    ),
    'all_relationships' => array(
      'label' => 'All Relationships',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_relationships',
      'table' => 'stock',
      'field' => 'all_relationships',
      'relationship' => 'none',
    ),
    'is_obsolete' => array(
      'label' => 'Is Obsolete',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'is_obsolete',
      'table' => 'stock',
      'field' => 'is_obsolete',
      'relationship' => 'none',
    ),
    'description' => array(
      'label' => 'Description',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'description',
      'table' => 'stock',
      'field' => 'description',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'genus' => array(
      'order' => 'ASC',
      'id' => 'genus',
      'table' => 'organism',
      'field' => 'genus',
      'relationship' => 'none',
    ),
    'species' => array(
      'order' => 'ASC',
      'id' => 'species',
      'table' => 'organism',
      'field' => 'species',
      'relationship' => 'none',
    ),
    'name' => array(
      'order' => 'ASC',
      'id' => 'name',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'order' => 'ASC',
      'id' => 'name_1',
      'table' => 'stock',
      'field' => 'name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'common_name' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'common_name_op',
        'identifier' => 'common_name',
        'label' => 'Organism Common Name',
        'bef_filter_description' => '',
        'bef_format' => 'bef',
        'remember' => 0,
        'bef_select_all_none' => 1,
        'bef_collapsible' => 0,
      ),
      'case' => 1,
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
    ),
    'type_id' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_id_op',
        'identifier' => 'type_id',
        'label' => 'Type',
        'bef_filter_description' => '',
        'bef_format' => 'bef',
        'remember' => 0,
        'bef_select_all_none' => 1,
        'bef_collapsible' => 0,
      ),
      'case' => 1,
      'id' => 'type_id',
      'table' => 'stock',
      'field' => 'type_id',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'crossing_block' => array(
      'operator' => '=',
      'value' => array(
        'year' => '',
        'season' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'operator' => '',
        'identifier' => 'crossing_block',
        'label' => 'Crossing Block (Progeny)',
        'bef_filter_description' => '',
        'optional' => 1,
        'single' => 1,
        'remember' => 0,
      ),
      'id' => 'crossing_block',
      'table' => 'stock',
      'field' => 'crossing_block',
      'relationship' => 'none',
      'cb_year' => '',
      'cb_season' => '',
    ),
    'name' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_op',
        'identifier' => 'name',
        'label' => 'Name Contains',
        'bef_filter_description' => '',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name',
      'table' => 'stock',
      'field' => 'name',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'search_results' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'search_results',
      'table' => 'views',
      'field' => 'search_results',
      'relationship' => 'none',
      'apply_button' => 'Search',
      'no_results_text' => 'Please filter the germplasm records so you see only the subset of interest. If you want to see all organisms simply leave all organisms unchecked. Then click "Search".',
    ),
    'is_germplasm' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'is_germplasm',
      'table' => 'stock',
      'field' => 'is_germplasm',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_stock content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Germplasm');
  $handler->override_option('empty', 'There are no germplasm matching the current filters. Please widen your search by removing some of the filter criteria.');
  $handler->override_option('empty_format', '2');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'views_dynamic_columns');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'summary' => '',
    'columns' => array(
      'nid' => 'nid',
      'uniquename' => 'uniquename',
      'name' => 'name',
      'cross_number' => 'cross_number',
      'properties' => 'properties',
      'name_1' => 'name_1',
      'common_name' => 'common_name',
      'crossing_block' => 'crossing_block',
      'parent_number' => 'parent_number',
      'relationships' => 'relationships',
      'relationships_1' => 'relationships_1',
      'relationships_2' => 'relationships_2',
      'all_dbxref' => 'all_dbxref',
      'all_properties' => 'all_properties',
      'all_relationships' => 'all_relationships',
      'is_obsolete' => 'is_obsolete',
      'description' => 'description',
    ),
    'info' => array(
      'nid' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 0,
        'hide' => 0,
      ),
      'uniquename' => array(
        'sortable' => 1,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'cross_number' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'properties' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'name_1' => array(
        'sortable' => 1,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'common_name' => array(
        'sortable' => 1,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'crossing_block' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'parent_number' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'relationships' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'relationships_1' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'relationships_2' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'all_dbxref' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'all_properties' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'all_relationships' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'is_obsolete' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'description' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_2');
  $handler->override_option('path', 'chado/germplasm');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => 'Germplasm',
    'description' => '',
    'weight' => '0',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));

  return $view;
}

/**
 * Default View: Progeny for a given Crossing Block
 * List all the progeny of a given crossing block; crossing block chosen in path
 *
 * @param $vars
 *   An array of variables for the default view to use including
 *     - cross_num_db_options
 *     - properties_synonym_options
 *     - properties_crossingblock_options
 *     - rels_maternal_options
 *     - rels_paternal_options
 *     - rels_maternal_type_id
 *     - rels_paternal_type_id
 * @return
 *   A default view object
 */
function germplasm_manage_crossingblock_progeny_default_view($vars) {

  $view = new view;
  $view->name = 'crossingblock_progeny';
  $view->description = 'List all the progeny of a given crossing block; crossing block chosen in path';
  $view->tag = 'chado default';
  $view->base_table = 'stock';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array(
    'nid' => array(
      'label' => 'Stock to Node',
      'required' => 0,
      'id' => 'nid',
      'table' => 'chado_stock',
      'field' => 'nid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('fields', array(
    'nid' => array(
      'label' => 'Nid',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'link_to_node' => 0,
      'exclude' => 1,
      'id' => 'nid',
      'table' => 'node',
      'field' => 'nid',
      'relationship' => 'none',
    ),
    'uniquename' => array(
      'label' => 'Unique Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'link_to_node' => 1,
      'id' => 'uniquename',
      'table' => 'stock',
      'field' => 'uniquename',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'stock',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'label' => 'Type',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'name_1',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'common_name' => array(
      'label' => 'Organism',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => 'node/[organism_nid]',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'relationship' => 'none',
    ),
    'dbxref' => array(
      'label' => 'Cross Number',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'stockprop_type_ids' => array(),
      'display_options' => array(
        'accession' => 'accession',
        'db_name' => 0,
        'link' => 0,
      ),
      'exclude' => 0,
      'id' => 'dbxref',
      'table' => 'stock',
      'field' => 'dbxref',
      'relationship' => 'none',
      'db_ids' => $vars['cross_num_db_options'],
    ),
    'properties' => array(
      'label' => 'Synonyms',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'stockprop_type_ids' => $vars['properties_synonym_options'],
      'stockprop_display_options' => array(
        'value' => 'value',
        'type' => 0,
      ),
      'exclude' => 0,
      'id' => 'properties',
      'table' => 'stock',
      'field' => 'properties',
      'relationship' => 'none',
    ),
    'all_dbxref' => array(
      'label' => 'All Database References',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_dbxref',
      'table' => 'stock',
      'field' => 'all_dbxref',
      'relationship' => 'none',
    ),
    'relationships' => array(
      'label' => 'Maternal Parent',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'stockrel_type_ids' => $vars['rels_maternal_options'],
      'stockrel_display_options' => array(
        0 => 'subject',
        1 => 'type',
        2 => 'object',
        'subject' => 'subject',
        'type' => 'type',
        'object' => 'object',
      ),
      'exclude' => 0,
      'id' => 'relationships',
      'table' => 'stock',
      'field' => 'relationships',
      'relationship' => 'none',
      'stockrel_display_parts' => array(
        'subject' => 'subject',
        'type' => 0,
        'object' => 0,
      ),
      'stockrel_display_rels' => 'object',
    ),
    'relationships_1' => array(
      'label' => 'Paternal Parent',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'stockrel_type_ids' => $vars['rels_paternal_options'],
      'stockrel_display_options' => array(
        0 => 'subject',
        1 => 'type',
        2 => 'object',
        'subject' => 'subject',
        'type' => 'type',
        'object' => 'object',
      ),
      'exclude' => 0,
      'id' => 'relationships_1',
      'table' => 'stock',
      'field' => 'relationships',
      'relationship' => 'none',
      'stockrel_display_parts' => array(
        'subject' => 'subject',
        'type' => 0,
        'object' => 0,
      ),
      'stockrel_display_rels' => 'object',
    ),
    'all_relationships' => array(
      'label' => 'All Relationships',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_relationships',
      'table' => 'stock',
      'field' => 'all_relationships',
      'relationship' => 'none',
    ),
    'all_properties' => array(
      'label' => 'All Properties',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_properties',
      'table' => 'stock',
      'field' => 'all_properties',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'name' => array(
      'order' => 'ASC',
      'id' => 'name',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'order' => 'ASC',
      'id' => 'name_1',
      'table' => 'stock',
      'field' => 'name',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('arguments', array(
    'genus' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => 0,
      'ignorecase' => 0,
      'limit' => '0',
      'case' => 'none',
      'path_case' => 'none',
      'transform_dash' => 0,
      'id' => 'genus',
      'table' => 'organism',
      'field' => 'genus',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_taxonomy_tid_term_page' => 0,
      'default_taxonomy_tid_node' => 0,
      'default_taxonomy_tid_limit' => 0,
      'default_taxonomy_tid_vids' => array(),
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => '',
    ),
    'properties' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '%1: %2 Crossing Block Progeny',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => FALSE,
      'limit' => 0,
      'case' => 'ucwords',
      'path_case' => 'ucwords',
      'transform_dash' => 1,
      'id' => 'properties',
      'table' => 'stock',
      'field' => 'properties',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_fixed' => '',
      'default_argument_user' => 0,
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => '',
      'stockprop_type_id' => '3663',
      'operator' => '=',
    ),
    'properties_1' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '%1: %2 %3 Crossing Block Progeny',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => FALSE,
      'limit' => 0,
      'case' => 'ucwords',
      'path_case' => 'ucwords',
      'transform_dash' => 1,
      'id' => 'properties_1',
      'table' => 'stock',
      'field' => 'properties',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_fixed' => '',
      'default_argument_user' => 0,
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => '',
      'stockprop_type_id' => '3664',
      'operator' => '=',
    ),
  ));
  $handler->override_option('filters', array(
    'type_id' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_id_op',
        'identifier' => 'type_id',
        'label' => 'Type',
        'bef_filter_description' => '',
        'bef_format' => 'bef',
        'remember' => 0,
        'bef_select_all_none' => 1,
        'bef_collapsible' => 0,
      ),
      'case' => 1,
      'id' => 'type_id',
      'table' => 'stock',
      'field' => 'type_id',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'name_1' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_1_op',
        'identifier' => 'name_1',
        'label' => 'Name Contains',
        'bef_filter_description' => '',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name_1',
      'table' => 'stock',
      'field' => 'name',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'relationships' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'label' => 'Maternal Parent',
        'identifier' => 'maternal_parent',
        'use_operator' => 0,
        'operator' => 'maternal_parent_op',
        'optional' => 1,
      ),
      'id' => 'relationships',
      'table' => 'stock',
      'field' => 'relationships',
      'relationship' => 'none',
      'type' => $vars['rels_maternal_type_id'],
      'fixed_position' => 'subject_id',
    ),
    'relationships_1' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'label' => 'Paternal Parent',
        'identifier' => 'paternal_parent',
        'use_operator' => 0,
        'operator' => 'relationships_op',
        'optional' => 1,
      ),
      'id' => 'relationships_1',
      'table' => 'stock',
      'field' => 'relationships',
      'relationship' => 'none',
      'type' => $vars['rels_paternal_type_id'],
      'fixed_position' => 'subject_id',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_stock content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Progeny');
  $handler->override_option('empty', 'Please use the filter the germplasm records so you see only the subset of interest.');
  $handler->override_option('empty_format', '2');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'views_dynamic_columns');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'uniquename' => 'uniquename',
      'name' => 'name',
      'name_1' => 'name_1',
      'common_name' => 'common_name',
      'properties' => 'properties',
      'dbxref' => 'dbxref',
      'all_dbxref' => 'all_dbxref',
      'relationships' => 'relationships',
      'relationships_1' => 'relationships_1',
      'all_relationships' => 'all_relationships',
      'all_properties' => 'all_properties',
    ),
    'info' => array(
      'uniquename' => array(
        'sortable' => 1,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'name_1' => array(
        'sortable' => 1,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'common_name' => array(
        'sortable' => 1,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'dbxref' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'all_dbxref' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'relationships' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'relationships_1' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'all_relationships' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'all_properties' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'Crossing_Block/%/%/%/Progeny');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));

  return $view;
}

/**
 * Default View: Progeny for a given Crossing Block
 * List all the parents of a given crossing block; crossing block chosen in path
 *
 * @param $vars
 *   An array of variables for the default view to use including
 *     - cross_num_db_options
 *     - properties_synonym_options
 *     - properties_crossingblock_options
 *     - rels_maternal_options
 *     - rels_paternal_options
 *     - rels_maternal_type_id
 *     - rels_paternal_type_id
 * @return
 *   A default view object
 */
function germplasm_manage_crossingblock_parents_default_view($vars) {

  $view = new view;
  $view->name = 'crossingblock_parents';
  $view->description = 'List all the parents of a given crossing block; crossing block chosen in path';
  $view->tag = 'chado default';
  $view->base_table = 'stock';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'parent_number' => array(
      'label' => 'Parent Number',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'display_options' => array(
        'parent_number' => 'parent_number',
        'crossing_block' => 0,
      ),
      'exclude' => 0,
      'id' => 'parent_number',
      'table' => 'stock',
      'field' => 'parent_number',
      'relationship' => 'none',
      'cb_year' => '0',
      'use_cbyear_arg' => 1,
      'cb_season' => '0',
      'use_cbseason_arg' => 1,
      'override' => array(
        'button' => 'Override',
      ),
      'link_to_cb_parent_view' => 0,
    ),
    'uniquename' => array(
      'label' => 'Unique Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'uniquename',
      'table' => 'stock',
      'field' => 'uniquename',
      'relationship' => 'none',
    ),
    'name' => array(
      'label' => 'Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'stock',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'name_1' => array(
      'label' => 'Type',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'name_1',
      'table' => 'cvterm',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'common_name' => array(
      'label' => 'Organism',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => 'node/[organism_nid]',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'common_name',
      'table' => 'organism',
      'field' => 'common_name',
      'relationship' => 'none',
    ),
    'properties' => array(
      'label' => 'Synonyms',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'stockprop_type_ids' => $vars['properties_synonym_options'],
      'stockprop_display_options' => array(
        'value' => 'value',
        'type' => 0,
      ),
      'exclude' => 0,
      'id' => 'properties',
      'table' => 'stock',
      'field' => 'properties',
      'relationship' => 'none',
    ),
    'parent_number_1' => array(
      'label' => 'Parent in Other Crossing Blocks',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'parent_number_1',
      'table' => 'stock',
      'field' => 'parent_number',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
      'display_options' => array(
        'crossing_block' => 'crossing_block',
        'parent_number' => 0,
      ),
      'cb_year' => '0',
      'use_cbyear_arg' => 0,
      'cb_season' => '0',
      'use_cbseason_arg' => 0,
      'link_to_cb_parent_view' => 1,
    ),
    'crossing_block' => array(
      'label' => 'Progeny in Crossing Block',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'stockprop_type_ids' => $vars['properties_crossingblock_options'],
      'stockprop_display_options' => array(
        0 => 'type',
        1 => 'value',
      ),
      'link_to_crossingblock' => 1,
      'cb_display_options' => array(
        'year' => 'year',
        'season' => 'season',
      ),
      'exclude' => 0,
      'id' => 'crossing_block',
      'table' => 'stock',
      'field' => 'crossing_block',
      'relationship' => 'none',
    ),
    'all_dbxref' => array(
      'label' => 'All Database References',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_dbxref',
      'table' => 'stock',
      'field' => 'all_dbxref',
      'relationship' => 'none',
    ),
    'all_properties' => array(
      'label' => 'All Properties',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_properties',
      'table' => 'stock',
      'field' => 'all_properties',
      'relationship' => 'none',
    ),
    'all_relationships' => array(
      'label' => 'All Relationships',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_relationships',
      'table' => 'stock',
      'field' => 'all_relationships',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'parent_number' => array(
      'order' => 'ASC',
      'id' => 'parent_number',
      'table' => 'stock',
      'field' => 'parent_number',
      'relationship' => 'none',
      'cb_year' => '0',
      'use_cbyear_arg' => 1,
      'cb_season' => '0',
      'use_cbseason_arg' => 1,
    ),
  ));
  $handler->override_option('arguments', array(
    'genus' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '% Crossing Blocks',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => 0,
      'ignorecase' => 0,
      'limit' => '0',
      'case' => 'none',
      'path_case' => 'none',
      'transform_dash' => 0,
      'id' => 'genus',
      'table' => 'organism',
      'field' => 'genus',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_taxonomy_tid_term_page' => 0,
      'default_taxonomy_tid_node' => 0,
      'default_taxonomy_tid_limit' => 0,
      'default_taxonomy_tid_vids' => array(),
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => '',
    ),
    'pnum_cb_year' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '%2 %1 Crossing Block Parents',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => 0,
      'ignorecase' => 0,
      'limit' => '0',
      'case' => 'none',
      'path_case' => 'none',
      'transform_dash' => 0,
      'id' => 'pnum_cb_year',
      'table' => 'stock',
      'field' => 'pnum_cb_year',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_taxonomy_tid_term_page' => 0,
      'default_taxonomy_tid_node' => 0,
      'default_taxonomy_tid_limit' => 0,
      'default_taxonomy_tid_vids' => array(),
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => '',
    ),
    'pnum_cb_season' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '%1 %2 %3 Crossing Block Parents',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => 0,
      'ignorecase' => 0,
      'limit' => '0',
      'case' => 'none',
      'path_case' => 'none',
      'transform_dash' => 0,
      'id' => 'pnum_cb_season',
      'table' => 'stock',
      'field' => 'pnum_cb_season',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_taxonomy_tid_term_page' => 0,
      'default_taxonomy_tid_node' => 0,
      'default_taxonomy_tid_limit' => 0,
      'default_taxonomy_tid_vids' => array(),
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => '',
    ),
  ));
  $handler->override_option('filters', array(
    'type_id' => array(
      'operator' => '=',
      'value' => array(),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'type_id_op',
        'identifier' => 'type_id',
        'label' => 'Type',
        'bef_filter_description' => '',
        'bef_format' => 'bef',
        'remember' => 0,
        'bef_select_all_none' => 1,
        'bef_collapsible' => 0,
      ),
      'case' => 1,
      'id' => 'type_id',
      'table' => 'stock',
      'field' => 'type_id',
      'relationship' => 'none',
      'values_form_type' => 'select',
      'multiple' => 1,
      'optional' => 0,
      'show_all' => 0,
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'name_1' => array(
      'operator' => '~',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'name_1_op',
        'identifier' => 'name_1',
        'label' => 'Name Contains',
        'bef_filter_description' => '',
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'name_1',
      'table' => 'stock',
      'field' => 'name',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
      'agg' => array(
        'records_with' => 1,
        'aggregates_with' => 1,
      ),
    ),
    'parent_number' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'parent_number',
      'table' => 'stock',
      'field' => 'parent_number',
      'relationship' => 'none',
      'cb_year' => '<NONE>',
      'use_cbyear_arg' => 1,
      'cb_season' => '<NONE>',
      'use_cbseason_arg' => 1,
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_stock content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', 'Parents');
  $handler->override_option('empty', 'Please use the filter the germplasm records so you see only the subset of interest.');
  $handler->override_option('empty_format', '2');
  $handler->override_option('items_per_page', 50);
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'views_dynamic_columns');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'parent_number' => 'parent_number',
      'uniquename' => 'uniquename',
      'name' => 'name',
      'name_1' => 'name_1',
      'common_name' => 'common_name',
      'properties' => 'properties',
      'parent_number_1' => 'parent_number_1',
      'crossing_block' => 'crossing_block',
      'all_dbxref' => 'all_dbxref',
      'all_properties' => 'all_properties',
      'all_relationships' => 'all_relationships',
      'properties_3' => 'properties_3',
      'properties_4' => 'properties_4',
      'properties_5' => 'properties_5',
      'properties_6' => 'properties_6',
      'properties_7' => 'properties_7',
      'properties_8' => 'properties_8',
      'properties_9' => 'properties_9',
      'properties_10' => 'properties_10',
      'properties_11' => 'properties_11',
      'properties_12' => 'properties_12',
      'properties_13' => 'properties_13',
      'properties_14' => 'properties_14',
      'properties_15' => 'properties_15',
      'properties_16' => 'properties_16',
      'properties_17' => 'properties_17',
      'properties_18' => 'properties_18',
      'properties_19' => 'properties_19',
    ),
    'info' => array(
      'parent_number' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'uniquename' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'name' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 0,
        'hide' => 0,
      ),
      'name_1' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'common_name' => array(
        'sortable' => 0,
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'parent_number_1' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 0,
      ),
      'crossing_block' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'all_dbxref' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'all_properties' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'all_relationships' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_3' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_4' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_5' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_6' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_7' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_8' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_9' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_10' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_11' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_12' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_13' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_14' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_15' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_16' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_17' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_18' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
      'properties_19' => array(
        'separator' => '',
        'add_remove' => 1,
        'hide' => 1,
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'Crossing_Block/%/%/%/Parents');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));

  return $view;
}

/**
 * Default View: Cross assigned parents
 * This view is used exclusively in the add/cross_parents form to show the user what
 * parents have already been assigned
 *
 * @param $vars
 *   An array of variables for the default view to use including
 *     - cross_num_db_options
 *     - properties_synonym_options
 *     - properties_crossingblock_options
 *     - rels_maternal_options
 *     - rels_paternal_options
 *     - rels_maternal_type_id
 *     - rels_paternal_type_id
 * @return
 *   A default view object
 */
function germplasm_manage_cross_assigned_parents_default_view($vars) {

  $view = new view;
  $view->name = 'cross_assigned_parents';
  $view->description = 'This view is used exclusively in the add/cross_parents form to show the user what parents have already been assigned';
  $view->tag = 'chado default';
  $view->base_table = 'stock';
  $view->core = 0;
  $view->api_version = '2';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'parent_number' => array(
      'label' => 'Parent Number',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'parent_number',
      'table' => 'stock',
      'field' => 'parent_number',
      'relationship' => 'none',
      'display_options' => array(
        'parent_number' => 'parent_number',
        'crossing_block' => 0,
      ),
      'cb_year' => '0',
      'use_cbyear_arg' => 1,
      'cb_season' => '0',
      'use_cbseason_arg' => 1,
    ),
    'uniquename' => array(
      'label' => 'Unique Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'uniquename',
      'table' => 'stock',
      'field' => 'uniquename',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'name' => array(
      'label' => 'Name',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'link_class' => '',
        'alt' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'name',
      'table' => 'stock',
      'field' => 'name',
      'relationship' => 'none',
    ),
    'all_properties' => array(
      'label' => 'All Properties',
      'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
      ),
      'empty' => '',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'hide_alter_empty' => 1,
      'type' => 'separator',
      'separator' => ', ',
      'exclude' => 0,
      'id' => 'all_properties',
      'table' => 'stock',
      'field' => 'all_properties',
      'relationship' => 'none',
      'format' => array(
        'format_string' => '[all_properties-cvterm_name]: [all_properties-value]',
      ),
    ),
    'properties' => array(
      'id' => 'properties',
      'table' => 'stock',
      'field' => 'properties',
      'exclude' => TRUE,
      'stockprop_type_ids' => $vars['properties_synonym_options'],
      'label' => 'Synonyms',
    ),
  ));
  $handler->override_option('sorts', array(
    'parent_number' => array(
      'order' => 'ASC',
      'id' => 'parent_number',
      'table' => 'stock',
      'field' => 'parent_number',
      'relationship' => 'none',
      'cb_year' => '0',
      'use_cbyear_arg' => 1,
      'cb_season' => '0',
      'use_cbseason_arg' => 1,
    ),
  ));
  $handler->override_option('arguments', array(
    'pnum_cb_year' => array(
      'default_action' => 'default',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'breadcrumb' => '',
      'default_argument_type' => 'php',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => 0,
      'limit' => '0',
      'case' => 'lower',
      'path_case' => 'lower',
      'transform_dash' => 0,
      'id' => 'pnum_cb_year',
      'table' => 'stock',
      'field' => 'pnum_cb_year',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_fixed' => '',
      'default_argument_user' => 0,
      'default_argument_php' => 'return $_SESSION[\'crossingblock_year\'];',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => '',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'pnum_cb_season' => array(
      'default_action' => 'default',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'breadcrumb' => '',
      'default_argument_type' => 'php',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'glossary' => 0,
      'limit' => '0',
      'case' => 'lower',
      'path_case' => 'lower',
      'transform_dash' => 0,
      'id' => 'pnum_cb_season',
      'table' => 'stock',
      'field' => 'pnum_cb_season',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_fixed' => '',
      'default_argument_user' => 0,
      'default_argument_php' => 'return $_SESSION[\'crossingblock_season\'];',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => '',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'organism_id' => array(
      'default_action' => 'default',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'breadcrumb' => '',
      'default_argument_type' => 'php',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'break_phrase' => 0,
      'not' => 0,
      'id' => 'organism_id',
      'table' => 'organism',
      'field' => 'organism_id',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_taxonomy_tid_term_page' => 0,
      'default_taxonomy_tid_node' => 0,
      'default_taxonomy_tid_limit' => 0,
      'default_taxonomy_tid_vids' => array(),
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_php' => 'return $_SESSION[\'organism_id\'];',
      'validate_argument_node_type' => array(),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_argument_vocabulary' => array(),
      'validate_argument_type' => 'tid',
      'validate_argument_transform' => 0,
      'validate_user_restrict_roles' => 0,
      'validate_argument_php' => '',
      'override' => array(
        'button' => 'Override',
      ),
    ),
  ));
  $handler->override_option('filters', array(
    'parent_number' => array(
      'operator' => '=',
      'value' => '',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'parent_number',
      'table' => 'stock',
      'field' => 'parent_number',
      'relationship' => 'none',
      'cb_year' => '<NONE>',
      'use_cbyear_arg' => 1,
      'cb_season' => '<NONE>',
      'use_cbseason_arg' => 1,
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'access chado_stock content',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('items_per_page', 0);
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'parent_number' => 'parent_number',
      'stock_nid' => 'stock_nid',
      'uniquename' => 'uniquename',
      'name' => 'name',
      'properties' => 'properties',
      'all_properties' => 'all_properties',
    ),
    'info' => array(
      'parent_number' => array(
        'separator' => '',
      ),
      'stock_nid' => array(
        'separator' => '',
      ),
      'uniquename' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'name' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'properties' => array(
        'separator' => '',
      ),
      'all_properties' => array(
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'node/add/crossingblock_parents/view');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));

  return $view;
}