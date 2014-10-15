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

    // Stock details
    // @see chado_stock_load()
    // -----------------------
    $values = array('stock_id' => $stock_id);
    $stock = chado_generate_var('stock', $values);
    $stock = chado_expand_var($stock, 'field', 'stock.uniquename');
    $stock = chado_expand_var($stock, 'field', 'stock.description');
    $nodes[$nid]->stock = $stock;

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

    // Now get the title
    $node->title = chado_get_node_title($node);

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

  // Options
  $organism_options = tripal_get_organism_select_options();
  $type_options = tripal_get_germplasm_type_select_options();

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
  $type_id = key($type_options);
  $organism_id = key($organism_options);
  $default_maternal = '';
  $current_maternal_desc = '';
  $default_paternal = '';
  $current_paternal_desc = '';
  $default_ril_cross_num = '';
  $default_date_registered = NULL;
  $default_market_class = '';
  $default_cb_year = '';
  $default_cb_season = 0;

  // 1) if we are editing an existing node then the stock is already part of the node
  if (property_exists($node, 'stock')) {
    $sname = $node->stock->name;
    $stock_id = $node->stock->stock_id;
    $type_id = $node->stock->type_id->cvterm_id;
    $organism_id = $node->stock->organism_id->organism_id;

    $maternal_parent = tripal_get_germplasm_parent('maternal',$node->stock);
    if (is_object($maternal_parent)) {
      if (isset($maternal_parent->nid)) {
        $default_maternal = '('.$maternal_parent->stock->stock_id.') '.$maternal_parent->stock->name;
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
        $default_paternal = '('.$paternal_parent->stock->stock_id.') '.$paternal_parent->stock->name;
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

    $properties = chado_generate_var('stockprop',array('stock_id' => $node->stock->stock_id), array('return_array' => TRUE));
    $properties = (is_array($properties)) ? $properties : array();
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

  $form['core']['organism_id'] = array(
    '#type' => 'select',
    '#title' => 'Scientific Name',
    '#description' => 'The genus and species of this germplasm.',
    '#options' => $organism_options,
    '#default_value' => $organism_id,
    '#required' => TRUE
  );

  $form['core']['type_id'] = array(
    '#type' => 'select',
    '#title' => 'Type',
    '#description' => 'The type best matching this germplasm.',
    '#options' => $type_options,
    '#default_value' => $type_id,
    '#required' => TRUE,
    '#ajax' => array(
      'callback' => 'ajax_tripal_germplasm_node_form_type_specific_options',
      'wrapper' => 'type-specific-options',
    ),
  );

  // TYPE-SPECIFIC OPTIONS
  //////////////////////////////////////

  $form['type_specific'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="type-specific-options">',
    '#suffix' => '</div>'
  );

  // Cross
  //-------------------------
  $form['type_specific']['cross'] = array(
    '#type' => 'fieldset',
    '#title' => 'Type-specific: Cross',
    '#weight' => 2,
    '#prefix' => '<div id="type-specific-options">',
    '#suffix' => '</div>'
  );
  if (!tripal_is_germplasm_cross(array('type_id' => $type_id))) {
    $form['type_specific']['cross']['#prefix'] = '<span style="display:none">';
    $form['type_specific']['cross']['#suffix'] = '</span>';
  }

  // Crossing Block Year
  $form['type_specific']['cross']['cb_year'] = array(
    '#type' => 'textfield',
    '#title' => 'Crossing Block Year',
    '#description' => 'The year this cross was made in (ie: '.date("Y").').',
    '#default_value' => $default_cb_year
  );

  $form['type_specific']['cross']['cb_season'] = array(
    '#type' => 'select',
    '#title' => 'Crossing Block Season',
    '#description' => 'The season this cross was made in.',
    '#options' => array(
      0 => '',
      'Spring' => 'Spring',
      'Summer' => 'Summer',
      'Fall' => 'Fall',
      'Winter' => 'Winter'
    ),
    '#default_value' => $default_cb_season
  );

  // Variety
  //-------------------------
  $form['type_specific']['variety'] = array(
    '#type' => 'fieldset',
    '#title' => 'Type-specific: Variety',
    '#weight' => 3,
  );
  if (!tripal_is_germplasm_variety(array('type_id' => $type_id))) {
    $form['type_specific']['variety']['#prefix'] = '<span style="display:none">';
    $form['type_specific']['variety']['#suffix'] = '</span>';
  }

  // Date Registered
  $form['type_specific']['variety']['date_registered'] = array(
    '#type' => 'textfield',
    '#title' => 'Date Registered',
    '#description' => 'The date this variety was registered.',
    '#default_value' => $default_date_registered
  );

  // Market Class
  $form['type_specific']['variety']['market_class'] = array(
    '#type' => 'textfield',
    '#title' => 'Market Class',
    '#description' => 'The market class of this variety.',
    '#default_value' => $default_market_class
  );

  // RIL
  //-------------------------
  $form['type_specific']['ril'] = array(
    '#type' => 'fieldset',
    '#title' => 'Type-specific: RIL',
    '#weight' => 4,
    '#prefix' => '<div id="type-specific-options">',
    '#suffix' => '</div>'
  );
  if (!tripal_is_germplasm_ril(array('type_id' => $type_id))) {
    $form['type_specific']['ril']['#prefix'] = '<span style="display:none">';
    $form['type_specific']['ril']['#suffix'] = '</span>';
  }

  // Original Cross Number
  $form['type_specific']['ril']['ril_cross_number'] = array(
    '#type' => 'textfield',
    '#title' => 'Original Cross Number',
    '#description' => 'The cross number the RIL originated from',
    '#default_value' => $default_ril_cross_num,
    '#autocomplete_path' => 'tripal_ajax/tripal_germplasm/name_to_id/ALL'
  );
  if (isset($node->stock->organism_id->genus)) {
    $form['type_specific']['ril']['ril_cross_number']['#autocomplete_path'] = 'tripal_ajax/tripal_germplasm/name_to_id/' . $node->stock->organism_id->genus;
  }

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
    '#collapsed' => !tripal_is_germplasm_accession(array('type_id' => $type_id)),
  );

  // Dbxrefs
  // @TODO: Create a compound form field to enter both the db.name and accession
*/

  // Parents
  //////////////////////////////////////
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
    '#autocomplete_path' => 'tripal_ajax/tripal_germplasm/name_to_id/ALL',
    '#weight' => -10
  );
  if (isset($node->stock->organism_id->genus)) {
    $form['parents']['maternal_parent']['#autocomplete_path'] = 'tripal_ajax/tripal_germplasm/name_to_id/' . $node->stock->organism_id->genus;
  }

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
    '#autocomplete_path' => 'tripal_ajax/tripal_germplasm/name_to_id/ALL',
    '#weight' => -9
  );
  if (isset($node->stock->organism_id->genus)) {
    $form['parents']['paternal_parent']['#autocomplete_path'] = 'tripal_ajax/tripal_germplasm/name_to_id/' . $node->stock->organism_id->genus;
  }

  // ADDITIONAL CHADO OPTIONS
  //////////////////////////////////////
  $form['chado_node_api'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 50
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
    'form_element_name' => 'sname',
    'cv_id' => tripal_get_germplasm_relationship_type_cv_id(),
  );

  // Finally, and add the additional form elements to the form
  chado_add_node_form_relationships($form, $form_state, $details);
  $form['relationships']['#weight'] = 1;
  $form['relationships']['#collapsible'] = TRUE;
  $form['relationships']['#collapsed'] = TRUE;
  $form['relationships']['#group'] = 'chado_node_api';
  unset($form['relationships']['#prefix']);
  unset($form['relationships']['#suffix']);

  // Use the germplasm specific autocomplete.
  if (isset($node->stock->organism_id->genus)) {
    $form['relationships']['relationship_table']['new']['object_name']['#autocomplete_path'] = 'tripal_ajax/tripal_germplasm/name_to_id/' . $node->stock->organism_id->genus;
    $form['relationships']['relationship_table']['new']['subject_name']['#autocomplete_path'] = 'tripal_ajax/tripal_germplasm/name_to_id/' . $node->stock->organism_id->genus;
  }
  else {
    $form['relationships']['relationship_table']['new']['object_name']['#autocomplete_path'] = 'tripal_ajax/tripal_germplasm/name_to_id/ALL';
    $form['relationships']['relationship_table']['new']['subject_name']['#autocomplete_path'] = 'tripal_ajax/tripal_germplasm/name_to_id/ALL';
  }

  // PROPERTIES FORM
  //---------------------------------------------
  $prop_cv = tripal_get_default_cv('stockprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    'fieldset_title' => 'Properties',
    'property_table' => 'stockprop',
    'chado_id' => $stock_id,
    'cv_id' => $cv_id
  );
  chado_add_node_form_properties($form, $form_state, $details);
  $form['properties']['#collapsible'] = TRUE;
  $form['properties']['#collapsed'] = TRUE;
  $form['properties']['#group'] = 'chado_node_api';
   unset($form['properties']['#prefix']);
  unset($form['properties']['#suffix']);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  $details = array(
    'linking_table' => 'stock_dbxref',
    'base_foreign_key' => 'stock_id',
    'base_key_value' => $stock_id
  );
  chado_add_node_form_dbxrefs($form, $form_state, $details);
  $form['addtl_dbxrefs']['#title'] = 'External Accessions';
  $form['addtl_dbxrefs']['#collapsible'] = TRUE;
  $form['addtl_dbxrefs']['#collapsed'] = TRUE;
  $form['addtl_dbxrefs']['#group'] = 'chado_node_api';
  unset($form['addtl_dbxrefs']['#prefix']);
  unset($form['addtl_dbxrefs']['#suffix']);

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

  // Ensure RIL Original Cross is based on autocomplete
  if (!isset($node->ril_cross_number_id) AND preg_match('/\((\d+)\).*/', $node->ril_cross_number, $matches)) {
    $form_state['values']['ril_cross_number_id'] = $matches[1];
    $node->ril_cross_number_id = $matches[1];
  }
}

/**
 * AJAX Callback: replace the Type-specific options when the type drop-down
 * on the node form is changed
 */
function ajax_tripal_germplasm_node_form_type_specific_options($form, $form_state) {
  return $form['type_specific'];
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

    // What we update depends on the type of germplasm we have. For example,
    // if this isn't a RIL we don't want to update the Original Cross number
    // selection since this might cause something not shown to the user to be re-added.
    $is_ril = tripal_is_germplasm_ril(array('type_id' => $node->type_id));
    $is_cross = tripal_is_germplasm_cross(array('type_id' => $node->type_id));
    $is_variety = tripal_is_germplasm_variety(array('type_id' => $node->type_id));

    // Update the properties
    // -----------------------
    $properties = chado_retrieve_node_form_properties($node);

    if ($is_cross) {
      // Crossing Block Year
      $type_id = tripal_get_germplasm_crossingblock_type_id('year');
      if (!empty($type_id)) {
        if (!empty($node->cb_year)) {
          $properties[$type_id] = array($node->cb_year);
        }
        else {
          unset($properties[$type_id]);
        }
      }
      else {
        drupal_set_message('Unable to save Crossing Block Year','error');
        tripal_report_error('tripal_germplasm',TRIPAL_ERROR,'Unable to save crossing block year when updating germplasm record. Specifically unable to find the crossingblock_year stock property');
      }
      // Crossing Block Season
      $type_id = tripal_get_germplasm_crossingblock_type_id('season');
      if (!empty($type_id)) {
        if (!empty($node->cb_season)) {
          $properties[$type_id] = array($node->cb_season);
        }
        else {
          unset($properties[$type_id]);
        }
      }
      else {
        drupal_set_message('Unable to save Crossing Block Season','error');
        tripal_report_error('tripal_germplasm',TRIPAL_ERROR,'Unable to save crossing block season when updating germplasm record. Specifically unable to find the crossingblock_season stock property');
      }
    }
    if ($is_variety) {
      // Variety Date Registered
      $type = tripal_get_cvterm(array('name' => 'variety_year_released'));
      if (isset($type->cvterm_id)) {
        $type_id = $type->cvterm_id;
        if (!empty($node->date_registered)) {
          $properties[$type_id] = array($node->date_registered);
        }
        else {
          unset($properties[$type_id]);
        }
      }
      // Market Class
      $type = tripal_get_cvterm(array('name' => 'market_class'));
      if (isset($type->cvterm_id)) {
        $type_id = $type->cvterm_id;
        if (!empty($node->market_class)) {
          $properties[$type_id] = array($node->market_class);
        }
        else {
          unset($properties[$type_id]);
        }
      }
    }
    $details = array(
      'property_table' => 'stockprop',
      'base_table' => 'stock',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $stock['stock_id']
    );
    chado_update_node_form_properties($node, $details,$properties);

    // Update the additional dbxrefs
    // -----------------------
    $details = array(
      'linking_table' => 'stock_dbxref',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $stock['stock_id']
    );
    chado_update_node_form_dbxrefs($node, $details);

    // Update relationships
    // -----------------------
    $relationships = chado_retrieve_node_form_relationships($node);

    if ($is_ril) {
      // RIL Original Cross Number
      // The RIL is_selection_of ORIGINAL CROSS
      $type = tripal_get_cvterm(array('name' => 'is_selection_of'));
      if (isset($type->cvterm_id)) {
        $type_id = $type->cvterm_id;
        if (!empty($node->ril_cross_number_id)) {
          // We need to find the existing relationship where the current
          // stock is the subject and update it
          $found = FALSE;
          if (isset($relationships[$type_id])) {
            foreach ($relationships[$type_id] as $id => $details) {
              if ($details['subject_id'] == $node->stock_id) {
                $found = TRUE;
                $relationships[$type_id][$id]['object_id'] = $node->ril_cross_number_id;
              }
            }
          }
          if (!$found) {
            $relationships[$type_id] = array(
              uniqid() => array(
                'subject_id' => $stock['stock_id'],
                'object_id' =>  $node->ril_cross_number_id
              )
            );
          }
        }
      }
    }
    $details = array(
      'relationship_table' => 'stock_relationship',
      'foreignkey_value' => $stock['stock_id']
    );
    chado_update_node_form_relationships($node, $details, $relationships);

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

    // What we update depends on the type of germplasm we have. For example,
    // if this isn't a RIL we don't want to update the Original Cross number
    // selection since this might cause something not shown to the user to be re-added.
    $is_ril = tripal_is_germplasm_ril(array('type_id' => $node->type_id));
    $is_cross = tripal_is_germplasm_cross(array('type_id' => $node->type_id));
    $is_variety = tripal_is_germplasm_variety(array('type_id' => $node->type_id));

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

    // Update the properties
    // -----------------------
    $properties = chado_retrieve_node_form_properties($node);

    if ($is_cross) {
      // Crossing Block Year
      $type_id = tripal_get_germplasm_crossingblock_type_id('year');
      if (!empty($type_id)) {
        if (!empty($node->cb_year)) {
          $properties[$type_id] = array($node->cb_year);
        }
        else {
          unset($properties[$type_id]);
        }
      }
      else {
        drupal_set_message('Unable to save Crossing Block Year','error');
        tripal_report_error('tripal_germplasm',TRIPAL_ERROR,'Unable to save crossing block year when updating germplasm record. Specifically unable to find the crossingblock_year stock property');
      }
      // Crossing Block Season
      $type_id = tripal_get_germplasm_crossingblock_type_id('season');
      if (!empty($type_id)) {
        if (!empty($node->cb_season)) {
          $properties[$type_id] = array($node->cb_season);
        }
        else {
          unset($properties[$type_id]);
        }
      }
      else {
        drupal_set_message('Unable to save Crossing Block Season','error');
        tripal_report_error('tripal_germplasm',TRIPAL_ERROR,'Unable to save crossing block season when updating germplasm record. Specifically unable to find the crossingblock_season stock property');
      }
    }
    if ($is_variety) {
      // Variety Date Registered
      $type = tripal_get_cvterm(array('name' => 'variety_year_released'));
      if (isset($type->cvterm_id)) {
        $type_id = $type->cvterm_id;
        if (!empty($node->date_registered)) {
          $properties[$type_id] = array($node->date_registered);
        }
        else {
          unset($properties[$type_id]);
        }
      }
      // Market Class
      $type = tripal_get_cvterm(array('name' => 'market_class'));
      if (isset($type->cvterm_id)) {
        $type_id = $type->cvterm_id;
        if (!empty($node->market_class)) {
          $properties[$type_id] = array($node->market_class);
        }
        else {
          unset($properties[$type_id]);
        }
      }
    }
    $details = array(
      'property_table' => 'stockprop',
      'base_table' => 'stock',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $node->stock_id
    );
    chado_update_node_form_properties($node, $details,$properties);

    // Update the additional dbxrefs
    // -----------------------
    $details = array(
      'linking_table' => 'stock_dbxref',
      'foreignkey_name' => 'stock_id',
      'foreignkey_value' => $node->stock_id
    );
    chado_update_node_form_dbxrefs($node, $details);

    // Update relationships
    // -----------------------
    $relationships = chado_retrieve_node_form_relationships($node);

    if ($is_ril) {
      // RIL Original Cross Number
      // The RIL is_selection_of ORIGINAL CROSS
      $type = tripal_get_cvterm(array('name' => 'is_selection_of'));
      if (isset($type->cvterm_id)) {
        $type_id = $type->cvterm_id;
        if (!empty($node->ril_cross_number_id)) {
          // We need to find the existing relationship where the current
          // stock is the subject and update it
          $found = FALSE;
          if (isset($relationships[$type_id])) {
            foreach ($relationships[$type_id] as $id => $details) {
              if ($details['subject_id'] == $node->stock_id) {
                $found = TRUE;
                $relationships[$type_id][$id]['object_id'] = $node->ril_cross_number_id;
              }
            }
          }
          if (!$found) {
            $relationships[$type_id] = array(
              uniqid() => array(
                'subject_id' => $node->stock_id,
                'object_id' =>  $node->ril_cross_number_id
              )
            );
          }
        }
      }
    }
    $details = array(
      'relationship_table' => 'stock_relationship',
      'foreignkey_value' => $node->stock_id
    );
    chado_update_node_form_relationships($node, $details, $relationships);

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
