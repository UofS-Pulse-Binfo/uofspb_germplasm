<?php

/**
 * @file
 * Additions to the Chado Stock node type to support germplasm better.
 */

/**
 * Load germplasm specific information to a node.
 */
function tripal_germplasm_node_load($nodes, $types) {

  if (in_array('chado_stock', $types)) {
    foreach($nodes as $nid => $node) {
      $node->germplasm = new stdClass();

      // Add the maternal & paternal parent for display in the overview.
      $maternal = tripal_get_germplasm_parent('maternal',$node->stock);
      if (is_object($maternal)) {
        $node->germplasm->maternal_parent = $maternal;
      }
      $paternal = tripal_get_germplasm_parent('paternal',$node->stock);
      if (is_object($paternal)) {
        $node->germplasm->paternal_parent = $paternal;
      }

      // Add all the relationships
      $progeny = tripal_get_germplasm_progeny($node->stock, 'all');
      if (!empty($progeny)) {
        usort($progeny, "tripal_germplasm_cmp_progeny_name");
        $node->germplasm->progeny = $progeny;
      }

      $nodes[$nid] = $node;
    }
  }

  return $nodes;
}

/**
 * Custom Sort: Sort progeny results by progeny name
 */
function tripal_germplasm_cmp_progeny_name($a, $b) {
  return strcmp($a['progeny']['name'], $b['progeny']['name']);
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Alter the stock node form to add germplasm specific fields and make it
 * easier to enter germplasm.
 */
function tripal_germplasm_form_chado_stock_node_form_alter(&$form, &$form_state, $form_id) {
  $form['#node']->stock = chado_expand_var($form['#node']->stock, 'table', 'stockprop', array('return_array' => TRUE));
  $node = $form['#node'];

  // Gather information about special fields.
  //---------------------------------------------
  $special_fields = module_invoke_all('special_germplasm_node_fields');
  $form_state['germplasm']['special_fields'] = $special_fields;
  //dpm($special_fields, 'special_fields');

  ///////////////////////////////////////////////
  // DEFAULT VALUES

  // 2) in the $form_state['values'] array which occurs on a failed validation or
  //    ajax callbacks from non submit form elements
  // 3) in the $form_state['input'] array which occurs on ajax callbacks from submit
  //    form elements and the form is being rebuilt
  $type_id = NULL;


  // 1) as elements of the $node object.  This occurs when editing an existing stock
  if (property_exists($node, 'stock')) {
    $sname = $node->stock->name;
    $stock_id = $node->stock->stock_id;
    $type_id = $node->stock->type_id->cvterm_id;
    $organism_id = $node->stock->organism_id->organism_id;
  }

  // 2) if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {

    $type_id = $form_state['values']['type_id'];

  }

  // 3) if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {

    $type_id = $form_state['values']['type_id'];

  }

  // Determine the category.
  $categories = module_invoke_all('germplasm_node_categories');
  $category = NULL;
  foreach ($categories as $machine_name => $details) {
    if (call_user_func($details['check_function'], array('type_id' => $type_id))) {
      $category = $machine_name;
    }
  }
  $form_state['germplasm']['category'] = $category;

  $defaults = array();
  $property_types = array();
  $relationship_types = array();
  foreach($special_fields as $machine_name => $details) {
    if (!$details['type_specific'] OR in_array($category, $details['category'])) {
      $defaults[$machine_name] = call_user_func( $details['value_function'], $node, $details);

      if ($details['node_api_category'] == 'property') {
        $property_types[$details['type_id']] = (isset($details['type_name'])) ? $details['type_name'] : $machine_name;
      }
      elseif ($details['node_api_category'] == 'relationship') {
        $relationship_types[$details['current_stock_side']][$details['type_id']] = (isset($details['type_name'])) ? $details['type_name'] : $machine_name;
      }
    }
  }
  $form_state['germplasm']['defaults'] = $defaults;
  $form_state['germplasm']['property_types'] = $property_types;
  $form_state['germplasm']['relationship_types'] = $relationship_types;
  //dpm($defaults, 'defaults');

  ///////////////////////////////////////////////
  // Form Proper.

  //---------------------------------------------
  // Move the core elements into a fieldset.
  $form['core'] = array(
    '#type' => 'fieldset',
    '#weight' => -10
  );

  $core_elements = array('sname', 'type_id', 'organism_id');
  foreach ($core_elements as $element_name) {
    $form['core'][$element_name] = $form[$element_name];
    unset($form[$element_name]);
  }

  // Hide notes.
  $form['stock_description']['#type'] = 'hidden';

  // Hide uniquename & main database reference since these will be set for them.
  $form['uniquename']['#type'] = 'hidden';
  $form['uniquename']['#required'] = FALSE;
  $form['uniquename']['#value'] = (isset($node->stock)) ? $node->stock->uniquename : uniqid('GERM:');
  $form['database_reference']['#type'] = 'hidden';

  // Add ajax to the type.
  $form['core']['type_id']['#ajax'] = array(
    'callback' => 'ajax_tripal_germplasm_node_form_type_specific_options',
    'wrapper' => 'type-specific-options',
  );

  // Chado node api changes.
  //---------------------------------------------
  // Set the weight.
  $form['chado_node_api']['#weight'] = 50;

  // change the name of the additional dbxrefs fieldset.
  $form['addtl_dbxrefs']['#title'] = 'External Accessions';

  // Make Chado Node API relationship table use our autocomplete.
  $form['relationships']['relationship_table']['new']['object_name']['#autocomplete_path'] = 'tripal_ajax/tripal_germplasm/name_to_id/' . $node->stock->organism_id->genus;
  $form['relationships']['relationship_table']['new']['subject_name']['#autocomplete_path'] = 'tripal_ajax/tripal_germplasm/name_to_id/' . $node->stock->organism_id->genus;

  // Add in some themeing
  //---------------------------------------------
  $form['#attached']['css'][] = drupal_get_path('module','tripal_germplasm') . '/theme/css/tripal_germplasm.node_form.css';
  $form['#attached']['js'][] = drupal_get_path('module','tripal_germplasm') . '/theme/js/stockNode_relationshipJump.js';
  $form['#attached']['js'][] = drupal_get_path('module','tripal_germplasm') . '/theme/js/stockNode_propertyJump.js';

  //---------------------------------------------
  // Type-specific Additions.
  $form['type_specific'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="type-specific-options">',
    '#suffix' => '</div>'
  );

  $form['type_specific'][$category] = array(
    '#type' => 'fieldset',
    '#title' => 'Type-specific: ' . $categories[$category]['title'],
    '#weight' => 2,
    '#prefix' => '<div id="type-specific-options">',
    '#suffix' => '</div>'
  );

  // Now add in each of the type-specific fields :).
  foreach($special_fields as $machine_name => $details) {
    if (in_array($category, $details['category']) AND $details['type_specific']) {

      $form['type_specific'][$category][$machine_name] = array(
        '#type' => $details['form_type'],
        '#title' => $details['title'],
        '#description' => $details['description'],
        '#default_value' => $defaults[$machine_name]['value']
      );

      if ($details['node_api_category'] == 'relationship') {
        $form['type_specific'][$category][$machine_name]['#autocomplete_path'] ='tripal_ajax/tripal_germplasm/name_to_id/' . $node->stock->organism_id->genus;
      }

    }
  }

  // Disable the rows in the chado node api: properties
  // that are now controlled by custom fields.
  //---------------------------------------------
  foreach ($property_types as $type_id => $property_name) {
    if (isset($form['properties']['property_table'][$type_id])) {
      foreach ($form['properties']['property_table'][$type_id] as $key => $form_row) {
        if (!preg_match('/^#/', $key)) {
          unset($form['properties']['property_table'][$type_id][$key]['property_action']['#ajax']);
          $form['properties']['property_table'][$type_id][$key]['type']['#prefix'] = '** ';
          $form['properties']['property_table'][$type_id][$key]['#attributes']['class'][] = 'custom-control';
        }
      }
    }
  }

  // And notify the user of what's going on.
  // Both in the type-specific fieldsets
  $form['type_specific'][$category]['removal-warning'] = array(
    '#type' => 'markup',
    '#markup' => 'Remove a type-specific property by deleting the text in the corresponding box below.',
    '#prefix' => '<div id="chado-node-api-property-parent-warning" class="messages property-type-specific fieldset-warning tripal-germplasm-warning" style="display:none; height: 18px">',
    '#suffix' => '</div>',
    '#weight' => -10
  );

  // And notifty them in the chado node api fieldset as well.
  $form['properties']['type_specific_msg'] = array(
    '#type' => 'item',
    '#markup' => '** To add or remove <em>Type-Specific Properties</em> of the current germplasm, please use the "Type-Specific" section above.',
    '#prefix' => '<div id="chado-node-api-relationship-parent-warning" class="messages property-type-specific tripal-germplasm-warning">',
    '#suffix' => '</div>',
    '#weight' => 4
  );

  // Add additional validate to Chado Node Api Property "Add" button
  $form['properties']['property_table']['new']['property_action']['#validate'][] = 'tripal_germplasm_node_form_properties_add_button_validate';


  //---------------------------------------------
  // Parents
  $form['parents'] = array(
    '#type' => 'fieldset',
    '#title' => 'Parents',
    '#description' => 'The parents of the current germplasm (if known).'
  );

  // Now add in the parental fields :).
  foreach($special_fields as $machine_name => $details) {
    if (in_array('parent', $details['category'])) {

      $form['parents'][$machine_name . '_id'] = array(
        '#type' => 'hidden',
        '#value' => $defaults[$machine_name]['stock_id']
      );

      $form['parents'][$machine_name] = array(
        '#type' => $details['form_type'],
        '#title' => $details['title'],
        '#description' => $details['description'],
        '#default_value' => $defaults[$machine_name]['value']
      );

      if ($details['node_api_category'] == 'relationship') {
        $form['parents'][$machine_name]['#autocomplete_path'] ='tripal_ajax/tripal_germplasm/name_to_id/' . $node->stock->organism_id->genus;
      }

    }
  }

  // Disable the rows in the chado node api: relationships
  // that are now controlled by custom fields.
  //---------------------------------------------
  // Ensure they don't remove relationships they shouldn't.
  foreach ($relationship_types as $side => $types) {
    foreach ($types as $type_id => $type_name) {
      foreach ($form['relationships']['relationship_table'][$type_id] as $key => $form_row) {
        if (!preg_match('/^#/', $key)) {
          if ($form_row[$side . '_id']['#value'] == $form_state['node']->stock->stock_id) {
            unset($form['relationships']['relationship_table'][$type_id][$key]['rel_action']['#ajax']);
            $form['relationships']['relationship_table'][$type_id][$key]['subject_name']['#prefix'] = '** ';
            $form['relationships']['relationship_table'][$type_id][$key]['#attributes']['class'][] = 'custom-control';
          }
        }
      }
    }
  }

  // And notify the user of what's going on.
  // Both in the parents fieldset
  $form['parents']['removal-warning'] = array(
    '#type' => 'markup',
    '#markup' => 'Remove a parent by deleting the text in the corresponding box below.',
    '#prefix' => '<div id="edit-parent-fieldset-relationship-warning" class="messages relationship-parent tripal-germplasm-warning fieldset-warning" style="display:none; height: 18px">',
    '#suffix' => '</div>'
  );

  // And notifty them in the chado node api fieldset as well.
  $form['relationships']['parent_msg'] = array(
    '#type' => 'item',
    '#markup' => '** To add or remove Parents of the current germplasm, please use the "Parents" section above.',
    '#prefix' => '<div id="chado-node-api-relationship-parent-warning" class="messages relationship-parent tripal-germplasm-warning">',
    '#suffix' => '</div>',
    '#weight' => 4
  );

  // Add additional validate to Chado Node Api Relationship "Add" button
  $form['relationships']['relationship_table']['new']['rel_action']['#validate'][] = 'tripal_germplasm_node_form_relationships_add_button_validate';

}

/**
 * AJAX Callback: replace the Type-specific options when the type drop-down
 * on the node form is changed
 */
function ajax_tripal_germplasm_node_form_type_specific_options($form, $form_state) {
  return $form['type_specific'];
}

/**
 * Validate for the Chado Node API: Properties Add Button.
 *
 * Purpose: To ensure that users don't add type-specific properties
 * and instead use the appropriate section.
 */
function tripal_germplasm_node_form_properties_add_button_validate($form, &$form_state) {

  $new_type_id = $form_state['values']['property_table']['new']['type'];

  // Ensure that if a type has been selected that the user is not trying
  // to add a type-specific property.
  if ($new_type_id != 0) {
    if (isset($form_state['germplasm']['property_types'][$new_type_id])) {
      $type_name = $form_state['germplasm']['property_types'][$new_type_id];
      form_set_error(
        'property_table][new][type',
        t('Cannot add %type_name property because it is type-specific. Instead, enter this value in the "Type-Specific" section as the %field_name.',
          array(
            '%type_name' => $type_name,
            '%field_name' => $form_state['germplasm']['special_fields'][$type_name]['title']))
      );
    }
  }
}

/**
 * Validate for the Chado Node API: Relationship Add Button.
 *
 * Purpose: To ensure that users don't add parental relationship
 * and instead use the parents section.
 */
function tripal_germplasm_node_form_relationships_add_button_validate($form, &$form_state) {

  // First get information about the new relationship.
  $new_type_name = $form_state['values']['relationship_table']['new']['type_name'];
  // Get details assuming the special fields key is the type.name
  $new_type_details = $form_state['germplasm']['special_fields'][$new_type_name];

  // The relationship types array is first keyed by object/subject and then each
  // one has a list of types that we care about in that particular direction.
  // For example, we only care about the maternal parent relationship in the
  // object direction since the other direction would refer to progeny rather
  // than parents.
  foreach($form_state['germplasm']['relationship_types'] as $side => $types) {

    // Check to see if the new relationship is in the current direction...
    if ($form_state['values']['relationship_table']['new'][$side . '_is_current'] == TRUE) {

      // and is one of the types we care about for this direction.
      if (in_array($new_type_name, $types)) {

        // If it is then stop the user from adding it since they should be
        // using the custom fields. Further customize the error message based
        // on whether they tried to add a parental or type-specific relationship.
        if ($new_type_details['type_specific']) {
          form_set_error('relationship_table][new][type_name',
            t('Cannot add %type_name relationship because it is type-specific. Instead, enter this value in the "Type-Specific" section as the %field_name.',
             array(
              '%type_name' => $form_state['values']['relationship_table']['new']['type_name'],
              '%field_name' => $new_type_details['title']
            ))
          );
        }
        else {
          form_set_error('relationship_table][new][type_name',
            t('Cannot add %type_name relationship because it is parental. Instead, enter this value in the "Parents" section as the %field_name.',
             array(
              '%type_name' => $form_state['values']['relationship_table']['new']['type_name'],
              '%field_name' => $new_type_details['title']
            ))
          );
        }
      }
    }
  }
}

/**
 * Chado Stock Node Form Validate
 *
 * Implements hook_node_validate().
 *
 * NOTE: In order to take advantage of the Chado Node API many of the form
 * elements added in tripal_germplasm_form_chado_stock_node_form_alter()
 * simply add/update their value in the property/dbxref/relationship_table
 * arrays added by the chado_stock_form().
 */
function tripal_germplasm_node_validate($node, $form, &$form_state) {
  if ($node->type == 'chado_stock') {

    // For each of the special fields associated with the germplasm node...
    foreach ($form_state['germplasm']['special_fields'] as $field_name => $details) {

      // First ensure this is a field present in the form.
      if ($details['type_specific'] === FALSE OR in_array($form_state['germplasm']['category'], $details['category'])) {

        // Handle Relationships.
        //-------------------------------------------------------------------------
        if ($details['node_api_category'] == 'relationship') {

          // First check that the field was set using the autocomplete
          // (if it's set at all, of course ;) ).
          if (!empty($form_state['values'][$field_name]) AND !preg_match('/\((\d+)\).*/', $form_state['values'][$field_name], $matches)) {
            form_set_error($field_name, t('You need to choose the %title from the autocomplete rather than just typing in the full name manually.', array('%title' => $details['title'])));
          }
          // Next, check it's set, assuming it uses autocomplete based on
          // the previous check.
          elseif (!empty($form_state['values'][$field_name])) {

            // Mock-up a blank relationship.
            $relationship = array(
              'subject_id' => NULL,
              'type_id' => $details['type_id'],
              'object_id' => NULL
            );

            // Set the parent based on the regex match (stock_id)
            $changing_side = ($details['current_stock_side'] == 'subject') ? 'object_id' : 'subject_id';
            $relationship[ $changing_side ] = $matches[1];

            // Now add/update the parent in the form_state.
            tripal_germplasm_add_relationship_chado_node_form_reltable(
              $form_state,
              $relationship,
              $changing_side
            );
          }
          // If the form element is empty, we need to remove
          // a pre-existing relationship.
          else {
            // No need to remove if it's not been saved yet ;).
            if (isset($node->stock_id)) {

              // Mock-up a blank relationship.
              $relationship = array(
                'subject_id' => NULL,
                'type_id' => $details['type_id'],
                'object_id' => NULL
              );

              // Set the current stock to be used by the remove function.
              $relationship[ $details['current_stock_side'] . '_id' ] = $node->stock_id;

              // Only then can we delete.
              tripal_germplasm_remove_relationship_chado_node_form_reltable(
                $form_state,
                $relationship,
                $details['current_stock_side'] . '_id'
              );
            }
          }
        }
        // Now handle properties.
        //-------------------------------------------------------------------------
        // NOTE: the following code assumes that there will only ever be one stockprop
        // value for each type (ie: the rank field will not be used). As such,
        // if you have two occurances of a property added through another method
        // and someone empties that field then both properties of that type will be removed.
        elseif ($details['node_api_category'] == 'property') {

          // If there is a value then add it to the Chado Node API property table.
          if (!empty($form_state['values'][$field_name])) {
            $form_state['values']['property_table'][ $details['type_id'] ] = array(array(
              'prop_type_id' => $details['type_id'],
              'prop_value' => $form_state['values'][$field_name],
              'prop_rank' => 'TEMP' . uniqid()
            ));
          }
          // Otherwise remove it.
          else {
            unset($form_state['values']['property_table'][ $details['type_id'] ]);
          }

        }
      }
    } //End of for each special field.

  } //End of if stock
}

/**
 * Implements hook_node_update().
 *
 * NOTE: We are replacing the uniquename with an accession based on the
 * stock_id and adding a primary dbxref.
 */
function tripal_germplasm_node_insert($node) {
  if ($node->type == 'chado_stock') {

    $stock_id = db_query("SELECT stock_id from chado_stock WHERE nid=:nid", array(':nid' => $node->nid))->fetchField();

    // Update the stock with the new uniquename
    // We can't set this initially since it depends on the stock_id
    // which is only available after the stock is created
    $stock = array();
    $prefix = variable_get('germplasm_prefix', 'GERM:');
    $stock['uniquename'] = $prefix . $stock_id;
    chado_update_record('stock', array('stock_id' => $stock_id), $stock);

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
        chado_update_record('stock', array('stock_id' => $stock_id), $stock);
      }
    }
  }
}

/**
 * Implements hook_node_view().
 *
 * Custom germplasm node themeing.
 */
function tripal_germplasm_node_view($node, $view_mode, $langcode) {

  // Remove the tabs for properties, sysnonyms and references
  // since they are included in the overview.
  unset(
    $node->content['tripal_stock_properties'],
    $node->content['tripal_stock_synonyms'],
    $node->content['tripal_stock_references']
  );
}