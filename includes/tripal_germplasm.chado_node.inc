<?php
/**
 * @file
 * Implements a Germplasm content type
 */

/**
 * Implements hook_node_info().
 * Registers a stock node type
 *
 * @return
 *   An array describing various details of the node
 *
 * @ingroup tripal_germplasm
 */
function tripal_germplasm_node_info() {
  return array(
    'tripal_germplasm' => array(
      'name' => t('Germplasm'),
      'base' => 'tripal_germplasm',
      'description' => t('The basic genetic material for any plant, used to develop new seed varieties. Within the germplasm are the basic characteristics that make plants what they are.'),
      'has_title' => TRUE,
      'has_body' => FALSE,
      'chado_node_api' => array(
        'base_table' => 'stock',
        'hook_prefix' => 'tripal_germplasm',
        'record_type_title' => array(
          'singular' => t('Germplasm'),
          'plural' => t('Germplasm')
        ),
        'sync_filters' => array(
          'type_id' => TRUE,
          'organism_id' => TRUE
        ),
      )
    ),
  );
}

/**
 * Implements hook_load().
 * Prepares the chado_stock node
 *
 * @param $node
 *   The basic node containing all variables common to all nodes
 *
 * @return
 *   A stock node containing all the variables from the basic node and all stock specific variables
 *
 * D7 @todo: Make optimizations to take advantage of $nodes
 *
 * @ingroup tripal_germplasm
 */
function tripal_germplasm_load($nodes) {

  $new_nodes = array();
  foreach ($nodes as $nid => $node) {
    // Get the stock_id
    $stock_id = db_query('SELECT stock_id FROM {tripal_germplasm} WHERE nid=:nid AND vid=:vid',array(':nid' => $node->nid,':vid' => $node->vid))->fetchField();
    $node->stock_id = $stock_id;

    // Add in the default stock details
    $tmp = chado_stock_load(array($node->nid => $node));
    $node = $tmp[$node->nid];

    $node->title = chado_get_node_title($node);

    // Germplasm-specific details
    // ---------------------------
    $node->germplasm = new stdClass();

    // Parents
    $maternal = tripal_get_germplasm_parent('maternal',$node->stock);
    if (is_object($maternal)) {
      $node->germplasm->maternal_parent = $maternal;
    }
    $paternal = tripal_get_germplasm_parent('paternal',$node->stock);
    if (is_object($paternal)) {
      $node->germplasm->paternal_parent = $paternal;
    }

    $new_nodes[$nid] = $node;
  }

  return $new_nodes;
}

/**
 * Implements hook_form().
 * Creates the main Add/Edit/Delete Form for tripal germplasm
 *
 * @param $node
 *   An empty node object on insert OR the current stock node object on update
 * @param $form_state
 *   The current state of the form
 *
 * @return
 *   A description of the form to be rendered by drupal_get_form()
 *
 * @ingroup tripal_germplasm
 */
function tripal_germplasm_form($node, $form_state) {
  $form = array();

  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object.  This occurs when editing an existing stock
  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  //
  // set form field defaults
  $sname = '';
  $stock_id = 0;
  $type_id = 0;
  $organism_id = 0;
  $default_maternal = '';
  $current_maternal_desc = '';
  $default_paternal = '';
  $current_paternal_desc = '';
  $default_ril_cross_num = '';
  $default_date_registered = NULL;
  $default_market_class = '';
  $default_cb_year = date("Y");
  $default_cb_season = 'Summer';

dpm($node, 'node');

  // 1) if we are editing an existing node then the stock is already part of the node
  if (property_exists($node, 'stock')) {
    $sname = $node->stock->name;
    $stock_id = $node->stock->stock_id;
    $type_id = $node->stock->type_id->cvterm_id;
    $organism_id = $node->stock->organism_id->organism_id;

    $maternal_parent = tripal_get_germplasm_parent('maternal',$node->stock);
    if (is_object($maternal_parent)) {
      if (isset($maternal_parent->nid)) {
        $default_maternal = '('.$maternal_parent->stock_id.') '.$maternal_parent->stock->name;
        $current_maternal_desc = ' The current maternal parent is <em>' . l($maternal_parent->stock->name, 'node/' . $maternal_parent->nid) . '</em>.';
      }
      else {
        $default_maternal = '('.$maternal_parent->stock_id.') '.$maternal_parent->name;
        $current_maternal_desc = ' The current maternal parent is <em>' . $maternal_parent->name . '</em>.';
      }
    }

    $paternal_parent = tripal_get_germplasm_parent('paternal',$node->stock);
    if (is_object($paternal_parent)) {
      if (isset($paternal_parent->nid)) {
        $default_paternal = '('.$paternal_parent->stock_id.') '.$paternal_parent->stock->name;
        $current_paternal_desc = ' The current paternal parent is <em>' . l($paternal_parent->stock->name, 'node/' . $paternal_parent->nid) . '</em>.';
      }
      else {
        $default_paternal = '('.$paternal_parent->stock_id.') '.$paternal_parent->name;
        $current_paternal_desc = ' The current paternal parent is <em>' . $paternal_parent->name . '</em>.';
      }
    }


    $rel = chado_generate_var(
      'stock_relationship',
      array(
        'type_id' => array('name' => 'is_selection_of'),
        'subject_id' => $node->stock->stock_id
      ),
      array(
        'include_fk' => array('object_id' => TRUE)
      )
    );
    if (isset($rel->object_id)) {
      $default_ril_cross_num = '(' . $rel->object_id->stock_id . ') ' . $rel->object_id->name;
    }

    $properties = chado_generate_var('stockprop',array('stock_id' => $node->stock->stock_id));
    dpm($properties, 'properties');
    foreach ($properties as $property) {
      switch ($property->type_id->name) {
        case 'variety_year_released':   
          $default_date_registered = $property->value;
          break;
        case 'market_class':
          $default_market_class = $property->value;
          break;
        case 'crossingblock_year':
          $default_cb_year = $property->value;
          break;
        case 'crossingblock_season':
          $default_cb_season = $property->value;
          break;
      }
    }
  }

  // 2) if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $sname = $form_state['values']['sname'];
    $stock_id = $form_state['values']['stock_id'];
    $type_id = $form_state['values']['type_id'];
    $organism_id = $form_state['values']['organism_id'];

    $default_ril_cross_num = $form_state['values']['ril_cross_number'];
    $default_date_registered = $form_state['values']['date_registered'];
    $default_market_class = $form_state['values']['market_class'];
    $default_cb_year = $form_state['values']['cb_year'];
    $default_cb_season = $form_state['values']['cb_season'];

  }

  // 3) if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $sname = $form_state['input']['sname'];
    $stock_id = $form_state['input']['stock_id'];
    $type_id = $form_state['input']['type_id'];
    $organism_id = $form_state['input']['organism_id'];

    $default_ril_cross_num = $form_state['input']['ril_cross_number'];
    $default_date_registered = $form_state['input']['date_registered'];
    $default_market_class = $form_state['input']['market_class'];
    $default_cb_year = $form_state['input']['cb_year'];
    $default_cb_season = $form_state['input']['cb_season'];
  }

  $form['core'] = array(
    '#type' => 'fieldset',
  );

  $form['core']['sname'] = array(
    '#type' => 'textfield',
    '#title' => 'Name',
    '#description' => 'The name of the germplasm.',
    '#required' => TRUE,
    '#default_value' => $sname
  );

  $organism_options = tripal_get_organism_select_options();
  $form['core']['organism_id'] = array(
    '#type' => 'select',
    '#title' => 'Scientific Name',
    '#description' => 'The genus and species of this germplasm.',
    '#options' => $organism_options,
    '#default_value' => $organism_id,
    '#required' => TRUE
  );

  $type_options = tripal_get_germplasm_type_select_options();
  $form['core']['type_id'] = array(
    '#type' => 'select',
    '#title' => 'Type',
    '#description' => 'The type best matching this germplasm.',
    '#options' => $type_options,
    '#default_value' => $type_id,
    '#required' => TRUE
  );

  // Parents
  //-------------------------
  $form['parents'] = array(
    '#type' => 'fieldset',
    '#title' => 'Parents',
    '#description' => 'The parents of the current germplasm (if known).'
  );
  
  if (isset($maternal_parent->stock_id)) {
    $form['parents']['maternal_parent_id'] = array(
      '#type' => 'hidden',
      '#value' => $maternal_parent->stock_id
    );
  }
  $form['parents']['maternal_parent'] = array(
    '#type' => 'textfield',
    '#title' => 'Maternal Parent',
    '#description' => 'The name of the maternal parent of this germplasm.' . $current_maternal_desc,
    '#default_value' => $default_maternal,
    '#autocomplete_path' => 'tripal_ajax/relationship_nodeform/stock/name/name_to_id',
    '#weight' => -10
  );

  if (isset($paternal_parent->stock_id)) {
    $form['parents']['paternal_parent_id'] = array(
      '#type' => 'hidden',
      '#value' => $paternal_parent->stock_id
    );
  }
  $form['parents']['paternal_parent'] = array(
    '#type' => 'textfield',
    '#title' => 'Paternal Parent',
    '#description' => 'The name of the paternal parent of this germplasm.' . $current_paternal_desc,
    '#default_value' => $default_paternal,
    '#autocomplete_path' => 'tripal_ajax/relationship_nodeform/stock/name/name_to_id',
    '#weight' => -9
  );

  // Relationships
  //-------------------------
  $details = array(
    'relationship_table' => 'stock_relationship',    // the name of the table linking additional dbxrefs to this node
    'nodetype' => 'Germplasm',
    'nodetype_plural' => 'Germplasm',
    'base_table' => 'stock',                         // the name of the chado table this node links to
    'base_foreign_key' => 'stock_id',                // key to link to the chado content created by this node
    'base_key_value' => $stock_id,                   // the value of the above key
    'fieldset_title' => 'Relationships',             // the non-translated title for this fieldset
    'additional_instructions' => '',                  // a non-stranslated string providing additional instructions
    'base_name_field' => 'name',
    'cv_id' => tripal_get_germplasm_relationship_type_cv_id()
  );

  // Finally, and add the additional form elements to the form
  chado_add_node_form_relationships($form, $form_state, $details);
  $form['relationships']['#weight'] = 1;
  $form['relationships']['#collapsible'] = TRUE;
  $form['relationships']['#collapsed'] = TRUE;

  // Cross
  //-------------------------
  $form['cross'] = array(
    '#type' => 'fieldset',
    '#title' => 'Cross Details',
    '#weight' => 2,
    '#collapsible' => TRUE,
    '#collapsed' => !tripal_is_germplasm_cross(array('type_id' => $type_id))
  );

  // Crossing Block Year
  $form['cross']['cb_year'] = array(
    '#type' => 'textfield',
    '#title' => 'Crossing Block Year',
    '#description' => 'The year this cross was made in (ie: '.date("Y").').',
    '#default_value' => $default_cb_year
  );

  $form['cross']['cb_season'] = array(
    '#type' => 'select',
    '#title' => 'Crossing Block Season',
    '#description' => 'The season this cross was made in.',
    '#options' => array(
      'Spring' => 'Spring',
      'Summer' => 'Summer',
      'Fall' => 'Fall',
      'Winter' => 'Winter'
    ),
    '#default_value' => $default_cb_season
  );
  
  // Variety
  //-------------------------
  $form['variety'] = array(
    '#type' => 'fieldset',
    '#title' => 'Variety Details',
    '#weight' => 3,
    '#collapsible' => TRUE,
    '#collapsed' => !tripal_is_germplasm_variety(array('type_id' => $type_id))
  );

  // Date Registered
  $form['variety']['date_registered'] = array(
    '#type' => 'textfield',
    '#title' => 'Date Registered',
    '#description' => 'The date this variety was registered.',
    '#default_value' => $default_date_registered
  );
  
  // Market Class 
  $form['variety']['market_class'] = array(
    '#type' => 'textfield',
    '#title' => 'Market Class',
    '#description' => 'The market class of this variety.',
    '#default_value' => $default_market_class
  );

  // RIL
  //-------------------------
  $form['ril'] = array(
    '#type' => 'fieldset',
    '#title' => 'RIL Details',
    '#weight' => 4,
    '#collapsible' => TRUE,
    '#collapsed' => !tripal_is_germplasm_ril(array('type_id' => $type_id))
  );


  // Original Cross Number
  $form['ril']['ril_cross_number'] = array(
    '#type' => 'textfield',
    '#title' => 'Original Cross Number',
    '#description' => 'The cross number the RIL originated from',
    '#default_value' => $default_ril_cross_num,
    '#autocomplete_path' => 'tripal_ajax/relationship_nodeform/stock/name/name_to_id'
  );
  
  // Date Deposited with number of lines
  // @TODO: Add compound form element with two textfields to enter two properties concurrently
  
  // Accession
  //-------------------------
  /**
  $form['accession'] = array(
    '#type' => 'fieldset',
    '#title' => 'Accession Details',
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => !tripal_is_germplasm_accession(array('type_id' => $type_id))
  );

  // Dbxrefs
  // @TODO: Create a compound form field to enter both the db.name and accession
*/

  // PROPERTIES FORM
  //---------------------------------------------
  $prop_cv = tripal_get_default_cv('stockprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    'fieldset_title' => 'Additional Properties',
    'property_table' => 'stockprop',
    'chado_id' => $stock_id,
    'cv_id' => $cv_id
  );
  chado_add_node_form_properties($form, $form_state, $details);
  $form['properties']['#collapsible'] = TRUE;
  $form['properties']['#collapsed'] = TRUE;
  
  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = array(
    'linking_table' => 'stock_dbxref',
    'base_foreign_key' => 'stock_id',
    'base_key_value' => $stock_id
  );
  chado_add_node_form_dbxrefs($form, $form_state, $details);
  $form['addtl_dbxrefs']['#collapsible'] = TRUE;
  $form['addtl_dbxrefs']['#collapsed'] = TRUE;

  return $form;
}

/**
 * Implements hook_validate().
 * Validate the input from the tripal_germplasm node form
 *
 * @param $node
 *   The current node including fields with the form element names and submitted values
 * @param $form
 *   A description of the form to be rendered by drupal_get_form()
 *
 * @ingroup tripal_germplasm
 */
function tripal_germplasm_validate(&$node, $form, &$form_state) {


  // Ensure that the maternal/paternal_parent_id is set based on the autocomplete
  if (!isset($node->maternal_parent_id) AND preg_match('/\((\d+)\).*/', $node->maternal_parent, $matches)) {
    $form_state['values']['maternal_parent_id'] = $matches[1];
    $node->maternal_parent_id = $matches[1];
  }
  if (!isset($node->paternal_parent_id) AND preg_match('/\((\d+)\).*/', $node->paternal_parent, $matches)) {
    $form_state['values']['paternal_parent_id'] = $matches[1];
    $node->paternal_parent_id = $matches[1];
  }

}

/**
 * Implements hook_insert().
 * Inserts data from tripal_germplasm_form() into drupal and chado
 *
 * @param $node
 *   The current node including fields with the form element names and submitted values
 *
 * @return
 *   TRUE if the node was successfully inserted into drupal/chado; FALSE otherwise
 *
 * @ingroup tripal_germplasm
 */
function tripal_germplasm_insert($node) {

  if (!property_exists($node, 'is_sync')) {
    // Create the chado stock
    $stock = array(
      'name' => $node->sname,
      'uniquename' => uniqid(),
      'organism_id' => $node->organism_id,
      'type_id' => $node->type_id
    );
    $stock = chado_insert_record('stock', $stock);

    // Update the stock with the new uniquename
    // We can't set this initially since it depends on the stock_id
    // which is only available after the stock is created
    $prefix = variable_get('germplasm_prefix', 'GERM:');
    $stock['uniquename'] = $prefix . $stock['stock_id'];
    chado_update_record('stock', array('stock_id' => $stock['stock_id']), $stock);

    // Generate the primary dbxref
    $db_id = variable_get('tripal_stock_main_dbxref_db', 0);
    if ($db_id > 0) {
      $dbxref = tripal_insert_dbxref(array(
        'db_id' => $db_id,
        'accession' => $stock['uniquename'],
      ));

      // Update the stock to point to the new dbxref
      if (is_object($dbxref)) {
        $stock['dbxref_id'] = $dbxref->dbxref_id;
        chado_update_record('stock', array('stock_id' => $stock['stock_id']), $stock);
      }
    }

    // Set the maternal parent by adding it to the relationships to be processed below
    $maternal_type_id = variable_get('germplasm_mparent_rel_type_id', NULL);
    if (isset($node->maternal_parent_id) AND $maternal_type_id) {
      // Remove any existing maternal parents
      if (isset($node->relationship_table[$maternal_type_id])) {
        foreach ($node->relationship_table[$maternal_type_id] as $k => $rel) {
          if ($rel['object_id'] == $stock['stock_id']) {
            unset($node->relationship_table[$maternal_type_id][$k]);
          }
        }
      }
      $node->relationship_table[$maternal_type_id][] = array(
        'object_id' => $stock['stock_id'],
        'subject_id' => $node->maternal_parent_id,
        'type_id' => $maternal_type_id
      );
    }
    // Set the paternal parent by adding it to the relationships to be processed below
    $paternal_type_id = variable_get('germplasm_pparent_rel_type_id', NULL);
    if (isset($node->paternal_parent_id) AND $paternal_type_id) {
      // Remove any existing paternal parents
      if (isset($node->relationship_table[$paternal_type_id])) {
        foreach ($node->relationship_table[$paternal_type_id] as $k => $rel) {
          if ($rel['object_id'] == $stock['stock_id']) {
            unset($node->relationship_table[$paternal_type_id][$k]);
          }
        }
      }
      $node->relationship_table[$paternal_type_id][] = array(
        'object_id' => $stock['stock_id'],
        'subject_id' => $node->paternal_parent_id,
        'type_id' => $maternal_type_id
      );
    }

    // @TODO: Actually save type-specific data (ie: market class, date released, etc.)
    
    // Update the properties
    $details = array(
      'property_table' => 'stockprop',
      'base_table' => 'stock',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $stock['stock_id']
    );
    chado_update_node_form_properties($node, $details);

    // Update the additional dbxrefs
    $details = array(
      'linking_table' => 'stock_dbxref',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $stock['stock_id']
    );
    chado_update_node_form_dbxrefs($node, $details);

    // Update relationships
    $details = array(
      'relationship_table' => 'stock_relationship',
      'foreignkey_value' => $stock['stock_id']
    );
    chado_update_node_form_relationships($node, $details);

  }
  else {
    $stock = array('stock_id' => $node->stock_id);
  }

  // Insert the record to associate this germplasm with it's stock
  $link = array(
    'stock_id' => $stock['stock_id'],
    'nid' => $node->nid,
    'vid' => $node->vid
  );
  drupal_write_record('tripal_germplasm', $link);


}

/**
 * Implements hook_update().
 * Handles Editing/Updating of main stock info
 *
 * NOTE: Currently just writes over all old data
 *
 * @param $node
 *   The current node including fields with the form element names and submitted values
 *
 * @return
 *   TRUE if the node was successfully updated in drupal/chado; FALSE otherwise
 *
 * @ingroup tripal_germplasm
 */
function tripal_germplasm_update($node) {

  $node->sname = trim($node->sname);

  if ($node->revision) {
    // there is no way to handle revisions in Chado but leave
    // this here just to make not we've addressed it.
  }

  // don't need to update the dbxref since it's based on the stock_id

  // Update stock
  $stock = array(
    'name' => $node->sname,
    'organism_id' => $node->organism_id,
    'type_id' => $node->type_id
  );
  chado_update_record('stock',array('stock_id' => $node->stock_id),$stock);

  if ($node->stock_id > 0) {

    // Set the maternal parent by adding it to the relationships to be processed below
    $maternal_type_id = variable_get('germplasm_mparent_rel_type_id', NULL);
    if (isset($node->maternal_parent_id) AND $maternal_type_id) {
      // Remove any existing maternal parents
      if (isset($node->relationship_table[$maternal_type_id])) {
        foreach ($node->relationship_table[$maternal_type_id] as $k => $rel) {
          if ($rel['object_id'] == $node->stock->stock_id) {
            unset($node->relationship_table[$maternal_type_id][$k]);
          }
        }
      }
      $node->relationship_table[$maternal_type_id][] = array(
        'object_id' => $node->stock->stock_id,
        'subject_id' => $node->maternal_parent_id,
        'type_id' => $maternal_type_id
      );
    }
    // Set the paternal parent by adding it to the relationships to be processed below
    $paternal_type_id = variable_get('germplasm_pparent_rel_type_id', NULL);
    if (isset($node->paternal_parent_id) AND $paternal_type_id) {
      // Remove any existing paternal parents
      if (isset($node->relationship_table[$paternal_type_id])) {
        foreach ($node->relationship_table[$paternal_type_id] as $k => $rel) {
          if ($rel['object_id'] == $node->stock->stock_id) {
            unset($node->relationship_table[$paternal_type_id][$k]);
          }
        }
      }
      $node->relationship_table[$paternal_type_id][] = array(
        'object_id' => $node->stock->stock_id,
        'subject_id' => $node->paternal_parent_id,
        'type_id' => $maternal_type_id
      );
    }

    // @TODO: Actually save type-specific data (ie: market class, date released, etc.)
    
    // Update the properties
    $details = array(
      'property_table' => 'stockprop',
      'base_table' => 'stock',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $node->stock_id
    );
    chado_update_node_form_properties($node, $details);

    // Update the additional dbxrefs
    $details = array(
      'linking_table' => 'stock_dbxref',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $node->stock_id
    );
    chado_update_node_form_dbxrefs($node, $details);

    // Update relationships
    $details = array(
      'relationship_table' => 'stock_relationship',
      'foreignkey_value' => $node->stock_id
    );
    chado_update_node_form_relationships($node, $details);

  }
}

/**
 * Implements hook_delete().
 * Handles deleting of tripal_germplasms
 *
 * NOTE: Currently deletes data -no undo or record-keeping functionality
 *
 * @param $node
 *   The current node including fields with the form element names and submitted values
 *
 * @return
 *   TRUE if the node was successfully deleted from drupal/chado; FALSE otherwise
 *
 * @ingroup tripal_germplasm
 */
function tripal_germplasm_delete($node) {

  // Set stock in chado: is_obsolete = TRUE
  chado_query("DELETE FROM {stock} WHERE stock_id = :stock_id", array(':stock_id' => $node->stock->stock_id));

  //remove Tripal Germplasm drupal node link for all revisions
  db_query("DELETE FROM {tripal_germplasm} WHERE nid = :nid", array(':nid' => $node->nid));

}

/**
 * Implements hook_node_view(). Acts on all content types.
 *
 * @ingroup tripal_germplasm
 */
function tripal_germplasm_node_view($node, $view_mode, $langcode) {

  switch ($node->type) {
    case 'tripal_germplasm':
      if ($view_mode == 'full') {

        $node->content['#tripal_generic_node_template'] = TRUE;

        $node->content['tripal_germplasm_base'] = array(
          '#markup' => theme('tripal_germplasm_base', array('node' => $node)),
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );

        $node->content['tripal_stock_collections'] = array(
          '#markup' => theme('tripal_stock_collections', array('node' => $node)),
          '#tripal_toc_id'    => 'collections',
          '#tripal_toc_title' => 'Stock Collections',
        );
        $node->content['tripal_stock_properties'] = array(
          '#markup' => theme('tripal_stock_properties', array('node' => $node)),
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_stock_references'] = array(
          '#markup' => theme('tripal_stock_references', array('node' => $node)),
          '#tripal_toc_id'    => 'references',
          '#tripal_toc_title' => 'Cross References',
        );
        $node->content['tripal_stock_relationships'] = array(
          '#markup' => theme('tripal_stock_relationships', array('node' => $node)),
          '#tripal_toc_id'    => 'relationships',
          '#tripal_toc_title' => 'Relationships',
        );
        $node->content['tripal_stock_synonyms'] = array(
          '#markup' => theme('tripal_stock_synonyms', array('node' => $node)),
          '#tripal_toc_id'    => 'synonyms',
          '#tripal_toc_title' => 'Synonyms',
        );
        $node->content['tripal_stock_publications'] = array(
          '#markup' => theme('tripal_stock_publications', array('node' => $node)),
          '#tripal_toc_id'    => 'publications',
          '#tripal_toc_title' => 'Publications',
        );
      }
      break;
  }
}

/**
 * Implements hook_chado_node_sync_create_new_node().
 * Used when sync'ing germplasm
 */
function tripal_germplasm_chado_node_sync_create_new_node($new_node, $record) {

  $new_node->is_sync = TRUE;

  $new_node->stock_id = $record->stock_id;
  $new_node->organism_id = $record->organism_id;
  $new_node->sname = $record->name;
  $new_node->uniquename = $record->uniquename;
  $new_node->type_id = $record->type_id;

  return $new_node;
}

/**
 * Implements hook_permission().
 */
function tripal_germplasm_permission() {
  $perms = array();

  $perms['access germplasm'] = array(
    'title' => 'Access Non-Crosses',
    'description' => 'Provides access to all germplasm that is not considered a cross.',
  );

  $perms['access crosses'] = array(
    'title' => 'Access Crosses',
    'description' => 'Provides access to breeding program specific germplasm such as crosses.',
    'retrict access' => TRUE,
  );

  $perms['access varieties'] = array(
    'title' => 'Access Varieties',
    'description' => 'Provides access to registered varieties.',
  );

  $perms['edit germplasm'] = array(
    'title' => 'Edit Germplasm',
    'description' => 'Allow users to update the information for particular germplasm.',
    'retrict access' => TRUE,
  );

  $perms['create germplasm'] = array(
    'title' => 'Create Germplasm',
    'description' => 'Allow users to create germplasm',
  );

  $perms['delete germplasm'] = array(
    'title' => 'Delete Germplasm',
    'description' => 'Allow users to delete germplasm.',
    'retrict access' => TRUE,
  );
  
  return $perms;
}

/**
 * Implements hook_node_access().
 */
function tripal_germplasm_node_access($node, $op, $account) {
  $type = is_string($node) ? $node : $node->type;

  // Determine the stock type
  if (isset($node->stock)) {
    if (is_object($node->stock->type_id)) {
      $stock_type = array('type_id' => $node->stock->type_id->cvterm_id);
    }
    else {
      $stock_type = array('type_id' => $node->stock->type_id);
    }
  }
  else {
    $stock_type = array();
  }

  // Actually determine permissions
  if ($type == 'tripal_germplasm') {

    // Create
    if ($op == 'create' && user_access('create germplasm', $account)) {
      return NODE_ACCESS_ALLOW;
    }
    
    // Edit
    if ($op == 'update' && user_access('edit germplasm', $account)) {
      return NODE_ACCESS_ALLOW;
    }
    
    // Delete
    if ($op == 'delete' && user_access('delete germplasm', $account)) {
      return NODE_ACCESS_ALLOW;
    }
    
    // Access Crosses
    if (tripal_is_germplasm_cross($stock_type) && user_access('access crosses', $account)) {
      return NODE_ACCESS_ALLOW;
    }
    
    // Access non-cross Germplasm
    if (!tripal_is_germplasm_cross($stock_type) && user_access('access germplasm', $account)) {
      return NODE_ACCESS_ALLOW;
    }
    
    // Access Varieties
    if (tripal_is_germplasm_variety($stock_type) && user_access('access varieties', $account)) {
      return NODE_ACCESS_ALLOW;
    }

    return NODE_ACCESS_DENY;
  }

  return NODE_ACCESS_IGNORE;
}
