<?php

/////////////////////////////////////////////////////////////////////////////
// @section Bulk Register Crosses Form
// Purpose: Allow users to register 10 crosses at a time
//   where a cross is a stock with a maternal and paternal parent
/////////////////////////////////////////////////////////////////////////////

/**
 * Purpose: Display all the forms needed to register crosses in bulk
 *
 * @return HTML displaying all the forms
 */
function germplasm_manage_register_crosses_page() {
  $output = '';

  $output .= drupal_get_form('set_crossing_block_form');

  if (!empty($_SESSION['organism_id']) && !empty($_SESSION['crossingblock_season']) && !empty($_SESSION['crossingblock_year'])) {
    $output .= drupal_get_form('bulk_register_crosses_form');
  }

  return $output;
}

/**
 * Implements hook_form
 * Purpose: This form allows users to enter up to 10 crosses (where a cross consists of a new stock with a
 *   maternal and paternal parent) at once. It basically executes the chado_stock_node_form and the
 *   tripal_stock_add_ONE_relationship_form for 2X for each cross entered.
 *
 * @params form_state: form_state array as described for drupal form api
 * @return form: form array as described for drupal form api
 */
function bulk_register_crosses_form($form_state) {
	$form = array();

	$form['cross_db_fieldset'] = array(
		'#type' => 'fieldset',
		'#title' => t('Cross Number Database'),
		'#description' => t('The Cross Number is stored as a Database Reference. This ensures that the cross numbers can be'
												.' tied together for a given species. This is critical if the cross number is only unique within'
												.' a species and yet you have multiple species within your database. If the cross database (usually'
												.' named by species) is not in the following drop-down list, contact the administrator.'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE
	);

	$db_options = variable_get( 'germplasm_cross_dbs', array() );
	$db_options[0] = 'Select a cross database';
	ksort($db_options);
	$form['cross_db_fieldset']['cross_db'] = array(
		'#type' => 'select',
		'#title' => t('Cross Database'),
		'#default_value' => $_SESSION['cross_database_id'],
		'#options' => $db_options,
		'#required' => TRUE,
	);

  $stock_organism_options_resource = chado_query("SELECT o.organism_id, o.abbreviation FROM stock s
    LEFT JOIN organism o ON o.organism_id=s.organism_id
    GROUP BY o.organism_id, o.abbreviation");
  while ($r = db_fetch_object($stock_organism_options_resource)) {
    $stock_organism_options[$r->organism_id] = $r->abbreviation;
  }
  if (!empty($_SESSION['organism_id'])) {

    $options = tripal_core_chado_select(
  	  'cvterm',
  	  array('cvterm_id', 'name'),
  	  array('cv_id'=> variable_get('chado_stock_types_cv', 'null') )
  	);
  	foreach ($options as $o) {
  	  $type_options[$o->cvterm_id] = $o->name;
  	}
    $type_options[0] = 'Select a Type';
    if ($node->nid == '') {
      $type_default = 0;
    }
    else {
      $type_default = $node->stock_type_id;
    }

    for ($i=1; $i<=10; $i++) {
      $form["cross-$i"] = array(
        '#type' => 'fieldset',
			  '#title' => t("Cross #$i"),
      );

      $form["cross-$i"]["crossnum-$i"] = array(
      	'#type' => 'textfield',
 		   	'#title' => t('U of S Cross Number'),
 		   	'#size' => 20,
      );

      $form["cross-$i"]["type-$i"] = array(
      	'#type' => 'select',
      	'#title' => t('Type'),
      	'#options' => $type_options,
      	'#default_value' => $type_default,
      );

      $form["cross-$i"]["maternal_parent-$i"] = array(
        '#type' => 'textfield',
        '#title' => t('Maternal Parent'),
  			'#size' => 40,
      );

      $form["cross-$i"]["paternal_parent-$i"] = array(
        '#type' => 'textfield',
        '#title' => t('Paternal Parent'),
  			'#size' => 40,
      );
    }

    $form['num_crosses'] = array(
      '#type' => 'value',
      '#value' => $i
    );

    $form['submit-more'] = array(
      '#type' => 'submit',
      '#value' => t('Add More')
    );

    $form['submit-finish'] = array(
      '#type' => 'submit',
      '#value' => t('Finish')
    );
  }

  return $form;
}

/**
 * Implements hook_form_validate
 * Purpose: To validate the user input
 *
 * @params form: form array as described for drupal form api
 * @params form_state: form_state array as described for drupal form api
 */
function bulk_register_crosses_form_validate($form, &$form_state) {

  if ($form_state['values']['cross_db'] < 1) {
    form_set_error('cross_db', 'Please choose a cross database');
  }

  // for each cross
  for ($i=1; $i<=10; $i++) {

    // don't validate if entire line blank
    if ( ($form_state['values']["type-$i"] == 0)
                 AND empty($form_state['values']["maternal_parent-$i"])
                        AND empty($form_state['values']["paternal_parent-$i"]) ) {
      continue;
    }

    //validate missing values
    $missing_values = FALSE;
    if (empty($form_state['values']["type-$i"]) OR ($form_state['values']["type-$i"] == 0) ) {
      form_set_error("type-$i", '#'.$i.': Cross Type Required');
      $missing_values = TRUE;
    } elseif (empty($form_state['values']["maternal_parent-$i"]) OR ($form_state['values']["maternal_parent-$i"] == '0') ) {
      form_set_error("maternal_parent-$i", '#'.$i.': Maternal Parent Required');
      $missing_values = TRUE;
    } elseif (empty($form_state['values']["paternal_parent-$i"]) OR ($form_state['values']["paternal_parent-$i"] == '0') ) {
      form_set_error("paternal_parent-$i", '#'.$i.': Paternal Parent Required');
      $missing_values = TRUE;
    }

    if ($missing_values) { continue; }

    //check parent boxes contain a valid value--------------
    $elements = array("maternal_parent-$i", "paternal_parent-$i");
    foreach ($elements as $element) {

      // if parent textfield contains P\d+ then change to <synoynm_prefix>-P\d+ where synonym_prefix is based on session year and season
      if (preg_match('/^P(\d+)$/', $form_state['values'][$element], $matches)) {
        $synonym_prefix = '';
        if (preg_match('/[Ss]ummer/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'Sm'.$_SESSION['crossingblock_year']; }
        if (preg_match('/[Ff]all/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'F'.$_SESSION['crossingblock_year']; }
        if (preg_match('/[Ww]inter/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'W'.$_SESSION['crossingblock_year']; }
        if (preg_match('/[Ss]pring/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'Sp'.$_SESSION['crossingblock_year']; }
        $form_state['values'][$element] = $synonym_prefix .'-P'. $matches[1];
      }

      //get stock_ids of matching stocks
      $results = tripal_stock_get_stock_by_name_identifier(
        $form_state['values'][$element],
        $_SESSION['organism_id']
      );

      if (sizeof($results) > 1) {
        $links= array();
        for ($i=0; $i<sizeof($results); $i++) { $links[] = l($i+1, "node/".$results[$i]->nid); }
        $message = "Too many lines match '".$form_state['values'][$element]."'! "
                 . "Please refine your input to match ONLY ONE line. <br>"
                 . "To aid in this process, here is the germplasm that match your initial input: "
                 .join(', ',$links);
        form_set_error($element, $message);
      } elseif (sizeof($results) < 1) {
        form_set_error($element, "There is no germplasm matching your input. Please check your input for typos and/or search the germplasm ".l('here', 'stocks'));
      } elseif (sizeof($results) == 1) {
        $form_state['values'][$element] = $results[0]->stock->stock_id;
      }

    } //end of foreach parent/element

    // determine whether a cross number was supplied without a cross database
    if (!empty($form_state['values']["crossnum-$i"])) {
    	if (preg_match('/none/', $form_state['values']['cross_db'])) {
    		form_set_error("crossnum-$i", 'Please select a cross database for this cross number to be associated with.');
    	}
    }
  } //end of foreach cross
}

/**
 * Implements hook_form_submit
 * Purpose: To add the 10 crosses specified
 *
 * @params form: form array as described for drupal form api
 * @params form_state: form_state array as described for drupal form api
 */
function bulk_register_crosses_form_submit($form, $form_state) {

	// Keep cross db as a session variable
	$_SESSION['cross_database_id'] = $form_state['values']['cross_db'];

  // Submit Each of the crosses
  module_load_include('inc', 'node', 'node.pages');
  global $user;

  //determine cross database
  $cross_db = tripal_db_get_db_by_name($form_state['values']['cross_db']);

  $skipped = array();
  for ($i=1; $i<=10; $i++) {

    // ensure all values entered, else skip
    if ( ($form_state['values']["type-$i"] == 0)
                  AND ($form_state['values']["maternal_parent-$i"] == 0)
              AND ($form_state['values']["paternal_parent-$i"] == 0) ) {
      $skipped[] = $i;
      continue;
    }

    //create main stock node by submitting form--------------------------------------------
    $stock_name = (!empty($form_state['values']["crossnum-$i"])) ? $form_state['values']["crossnum-$i"] : "stock$i";
    $node = array();
    $node = array(
        'uid' => (string) $user->uid,
        'name' => (string) $user->name,
        'type' => 'chado_stock',
        'language' => '',
        'title' => $stock_name,
        'uniquename' => $stock_name,
        'type_id' => $form_state['values']["type-$i"],
        'organism_id' => $_SESSION["organism_id"],
        'simulate_multipart' => FALSE,
    );


    $node_form_state = array();
    $node_form_state = array(
      'values' => array(
        'type_id' => $form_state['values']["type-$i"],
        'organism_id' => $_SESSION["organism_id"],
        'uniquename' => $stock_name,
        'title' => $stock_name,
        'simulate_multipart' => FALSE,
        'op' => 'Save'
      ),
    );
    drupal_execute('chado_stock_node_form', $node_form_state, (object)$node);
    $node = node_load($node_form_state['nid']);

    // Create Main Db reference------------------------------------------------------
    $uniquename = variable_get( 'germplasm_prefix', 'GERM:') . $node->stock->stock_id;
    $insert_values = array(
      'accession' => $uniquename,
      'db_id' => variable_get('tripal_stock_main_dbxref_db', 0),
      'version' => 1
    );
    $success = tripal_core_chado_insert('dbxref', $insert_values);

    if ($success) {
    // Update name/title = uniquename-----------------------------------------------
      $update_values = array(
        'uniquename' => $uniquename,
        'dbxref_id' => array(
          'accession' => $uniquename,
          'db_id' => variable_get('tripal_stock_main_dbxref_db', 0)
        )
      );
      $select_values = array('stock_id' => $node->stock->stock_id);
      $success = tripal_core_chado_update('stock', $select_values, $update_values);
      if ($success) {
        drupal_set_message('Successfully Added Main Database Reference');
      } else {
        drupal_set_message('Unable to attach main database reference!', 'error');
      }
    } else {
      drupal_set_message('Unable to add main database reference!', 'error');
    }

    // Create crossing block properties-----------------------------------------------
    // Year
    $values = array(
      'stock_id' => $node->stock->stock_id,
      'type_id' => array(
        'cv_id' => variable_get('chado_stock_prop_types_cv', 'null'),
        'name' => 'crossingblock_year'
      ),
      'value' => $_SESSION["crossingblock_year"],
      'rank' => 1,
    );
    $success = tripal_core_chado_insert('stockprop', $values);
    if ($success) {
      drupal_set_message('Successfully Assigned Crossing Block Year');
    } else {
      drupal_set_message('Unable to Assing Crossing Block Year', 'error');
    }

    // Season
    $values = array(
      'stock_id' => $node->stock->stock_id,
      'type_id' => array(
        'cv_id' => variable_get('chado_stock_prop_types_cv', 'null'),
        'name' => 'crossingblock_season'
      ),
      'rank' => 1,
      'value' => $_SESSION["crossingblock_season"],
    );
    $success = tripal_core_chado_insert('stockprop', $values);
    if ($success) {
      drupal_set_message('Successfully Assigned Crossing Block Season');
    } else {
      drupal_set_message('Unable to Assing Crossing Block Season', 'error');
    }

    // Add cross number dbxref---------------------------------------------------------
    if (!empty($form_state['values']["crossnum-$i"])) {
      $values = array(
        'db_id' => $form_state['values']['cross_db'],
        'accession' => $form_state['values']["crossnum-$i"],
      );
      // check if dbxref already exists
      $dbxref = tripal_core_chado_select('dbxref',array('dbxref_id'), $values);
      if ($dbxref) {
        $success = true;
      } else {
        //attempt to create dbxref
        $values['version'] = ' ';
        $success = tripal_core_chado_insert('dbxref', $values);
      }
      // connect dbxref to stock using stock_dbxref table
      if ($success) {
        $values = array(
          'dbxref_id' => array(
            'db_id' => $form_state['values']['cross_db'],
            'accession' => $form_state['values']["crossnum-$i"],
          ),
          'stock_id' => $node->stock->stock_id
        );
        $success = tripal_core_chado_insert('stock_dbxref', $values);
        if ($success) {
          drupal_set_message('Successfully Added Cross Number (Database Reference)');
        } else {
          drupal_set_message('Unable to Attach Cross Number (Database Refrence) to Stock.', 'error');
        }
      } else {
        drupal_set_message('Unable to Create Cross Number (Database Reference)', 'error');
      }
    }

    // Create Maternal Relationship----------------------------------------------------
    $values = array(
      'subject_id' => $form_state['values']["maternal_parent-$i"],
      'type_id' => array(
        'cv_id' => variable_get('chado_stock_relationship_cv', 'null'),
        'name' => 'is_maternal_parent_of',
      ),
      'object_id' => $node->stock->stock_id,
      'rank' => 1
    );
    $success = tripal_core_chado_insert('stock_relationship', $values);
    if ($success) {
      drupal_set_message('Successfully Added Maternal Relationship');
    } else {
      drupal_set_message('Unable to add Maternal Parent','error');
    }

		// Create Paternal Relationship----------------------------------------------------
    $values = array(
      'subject_id' => $form_state['values']["paternal_parent-$i"],
      'type_id' => array(
        'cv_id' => variable_get('chado_stock_relationship_cv', 'null'),
        'name' => 'is_paternal_parent_of',
      ),
      'object_id' => $node->stock->stock_id,
      'rank' => 1
    );
    $success = tripal_core_chado_insert('stock_relationship', $values);
    if ($success) {
      drupal_set_message('Successfully Added Paternal Relationship');
    } else {
      drupal_set_message('Unable to add Paternal Parent','error');
    }

  } //end of for each one added

  drupal_set_message('Skipping Blank Entries: #'.implode(', #', $skipped).'.');

  if ($form_state['clicked_button']['#value'] == 'Add More') {
    drupal_goto('node/add/crosses');
  } else {
  	$organism = tripal_core_chado_select(
  	  'organism',
  	  array('genus'),
  	  array('organism_id'=>$_SESSION["organism_id"])
  	);
    $organism_common_name = preg_replace('/\s+/', '-', $organism[0]->genus);
    drupal_goto('Crossing_Block/'.$organism_common_name.'/'.$_SESSION["crossingblock_year"].'/'.$_SESSION["crossingblock_season"].'/Summary');
  }

}

/**
 * Implements theme_form()
 * Purpose: Provide themeing for bulk_register_crosses_form
 *
 * @params form: the form to be themed
 * @return output: the html markup which is the rendered form
 */
function theme_bulk_register_crosses_form($form) {
  $output = '';

	$output .= drupal_render($form['cross_db_fieldset']);

  $output .= '<table>';
  $output .= '<tr><th>Cross Number</th><th>Type</th><th>Maternal Parent</th><th>Paternal Parent</th><th></th></tr>';
  for ($i=1; $i<=10; $i++) {
    unset($form["cross-$i"]["crossnum-$i"]['#title']);
    unset($form["cross-$i"]["type-$i"]['#title']);
    unset($form["cross-$i"]["maternal_parent-$i"]['#title']);
    unset($form["cross-$i"]["paternal_parent-$i"]['#title']);

    $output .= '<tr><td>'.drupal_render($form["cross-$i"]["crossnum-$i"])
               .'</td><td>'.drupal_render($form["cross-$i"]["type-$i"])
             .'</td><td>'.drupal_render($form["cross-$i"]["maternal_parent-$i"])
         .'</td><td>'.drupal_render($form["cross-$i"]["paternal_parent-$i"])
         .'</td><td>'.drupal_render($form["cross-$i"]["marrowfat-$i"])
         .'</td></tr>';

    unset($form["cross-$i"]);
  }
  $output .= '</table>';

  $output .= drupal_render($form);

  return $output;

}