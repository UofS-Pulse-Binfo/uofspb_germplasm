<?php

/**
 * @Purpose: This form allows researchers to easily create crossing blocks
 *
 * This page is a composite of listing current parents for a given crossing block,
 * assigning alias' to already existing stocks to make them parents and creating
 * stocks simulatious to assign them as parents for a given crossing block.
 */
function assign_crossing_parents_page() {
  $output = '';

  $output .= drupal_get_form('set_crossing_block_form');

  // if the crossing block is set
  if (!empty($_SESSION['organism_id']) && !empty($_SESSION['crossingblock_season']) && !empty($_SESSION['crossingblock_year'])) {

    $output .= drupal_get_form('create_parent_alias_for_existing_stock_form');
    $output .= drupal_get_form('create_stock_and_parent_alias_form');

    $view = views_get_view('cross_assigned_parents');
    $output .= '<br /><br /><div id="feature-references" class="tripal_feature-info-box">';
    $output .= '<div class="tripal_expandableBox"><h3>Current Crossing Block Parents</h3></div>';
    $output .= '<div class="tripal_expandableBoxContent">';

    if ($view) {
      $output .= $view->execute_display('page_1');
    } else {
      drupal_set_message('View cross_assigned_parents doesn\'t exist and is required to see which parents have already been added to the current crossing block.','error');
    }
    $output .= '</div></div>';
  }

  return $output;
}

////////////////////////////////////////////////////////////////////////////////
// @section Add Parent Alias Form
////////////////////////////////////////////////////////////////////////////////

/**
 * Implements hook_form()
 * @purpose: this form assigns an alias/synonym property to a
 * selected stock based on the crossing block selected
 */
function create_parent_alias_for_existing_stock_form($form_state) {
  $form = array();

  if (!empty($_SESSION['organism_id'])) {
    $form['existing_parent'] = array(
      '#type' => 'fieldset',
      '#title' => t('Assing an Already Existing Germplasm'),
    );

    $form['existing_parent']['stock_id'] = array(
      '#type' => 'textfield',
      '#title' => t('Existing Germplasm'),
      '#size' => 60,
      '#required' => TRUE,
      '#description' => 'This is the line you want to add as a parent for the current crossing block. You can enter any unique identifier potentially including name, uniquename, and synonyms.',
    );

    $synonym_prefix = '';
    if (preg_match('/[Ss]ummer/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'Sm'.$_SESSION['crossingblock_year'].'-P'; }
    if (preg_match('/[Ff]all/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'F'.$_SESSION['crossingblock_year'].'-P'; }
    if (preg_match('/[Ww]inter/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'W'.$_SESSION['crossingblock_year'].'-P'; }
    if (preg_match('/[Ss]pring/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'Sp'.$_SESSION['crossingblock_year'].'-P'; }
    $form['existing_parent']['synonym_prefix'] = array(
      '#type' => 'hidden',
      '#value' => $synonym_prefix
    );

    $form['existing_parent']['parent_number'] = array(
      '#type' => 'textfield',
      '#title' => t('Parent Number'),
      '#size' => 15,
      '#required' => TRUE,
      '#description' => 'This is a unique (to this crossing block) number to identify this parent',
    );

    $form['existing_parent']['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Add as Parent of Crossing Block',
    );

  } //end of if no organism

  return $form;
}

/**
 * Implements hook_form_validate()
 */
function create_parent_alias_for_existing_stock_form_validate ($form, &$form_state) {

  if (!preg_match('/^[0-9]+$/', $form_state['values']['parent_number'])) {
    form_set_error('parent_number', 'The parent number must be an integer from 0-999');
  }

  $results = tripal_stock_get_stock_by_name_identifier($form_state['values']['stock_id'], $_SESSION['organism_id']);
  if (sizeof($results) > 1) {
      $links= array();
      for ($i=0; $i<sizeof($results); $i++) { $links[] = l($i+1, "node/".$results[$i]->nid); }
      $message = "Too many lines match '".$form_state['values']['stock_id']."'! "
                 . "Please refine your input to match ONLY ONE line. <br>"
                 . "To aid in this process, here are the lines that match your initial input: "
                 .join(', ',$links);
      form_set_error('stock_id', $message);
  } elseif (sizeof($results) < 1) {
      form_set_error('stock_id', "There are no stocks matching your input. Please check your input for typos and/or search the germplasm ".l('here', 'stocks'));
  } elseif (sizeof($results) == 1) {
      $form_state['values']['stock_id'] = $results[0]->stock->stock_id;
      $form_state['values']['stock_name'] = $results[0]->stock->name;
      $form_state['values']['stock_nid'] = $results[0]->nid;
  }

}

/**
 * Implements hook_form_submit()
 */
function create_parent_alias_for_existing_stock_form_submit($form, $form_state) {

  $synonym = $form_state['values']['synonym_prefix'].$form_state['values']['parent_number'];
  $rank = 1+ tripal_get_max_chado_rank(
    'stockprop',
    array(
      'stock_id'=> array(
        'type'=>'INT',
        'value'=>$form_state['values']['stock_id']
       )
    )
  );
  $insert_values = array(
    'stock_id' => $form_state['values']['stock_id'],
    'type_id' => array(
      'name' => 'synonym',
      'cv_id' => variable_get('chado_stock_prop_types_cv', 'null')
    ),
    'value' => $synonym,
    'rank' => $rank
  );
  $success = tripal_core_chado_insert('stockprop', $insert_values);
  if ($success) {
    drupal_set_message('Added synonym ('.$synonym.') to '.l($form_state['values']['stock_name'], 'node/'.$form_state['values']['stock_nid']).' which designates it as a parent of the current crossing block');
  } else {
    drupal_set_message('Unable to Assign Parent', 'error');
  }
}

/**
 * Themeing function for create_parent_alias_for_existing_stock_form
 */
function theme_create_parent_alias_for_existing_stock_form($form) {
  $output = '';

  $output .= '<fieldset><legend>Already Existing Germplasm</legend>';
  $output .= '<p>If the germplasm you wish to assign as a parent to the current crossing block already exists, '
         .'select it below and enter the parent number. This will add a synonym to that germplasm of the form '
       .'(season abbreviation)(year)-P(parent number)</p>';

  $output .= '<table>';
  $output .= '<tr><td>'.drupal_render($form['existing_parent']['stock_id'])
         .'</td><td>'.drupal_render($form['existing_parent']['synonym_prefix'])
       .'</td><td>'.drupal_render($form['existing_parent']['parent_number'])
       .'</td></tr>';
  $output .= '</table>';

  $output .= drupal_render($form['existing_parent']['submit']);

  $output .= '</fieldset>';
  unset($form['existing_parent']);
  $output .= drupal_render($form);

  return $output;
}

////////////////////////////////////////////////////////////////////////////////
// @section Create Stock and Add Parent Alias Form
////////////////////////////////////////////////////////////////////////////////

/**
 * Implements hook_form()
 * @purpose: this form creates a stock and assigns an alias/synonym property to
 * the newly created stock
 */
function create_stock_and_parent_alias_form($form_state) {
  $form = array();

  $form['create_parent'] = array(
    '#type' => 'fieldset',
    '#title' => 'Create New Germplasm',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['create_parent']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name'),
    '#description' => 'This name should be the first given to this Individual/Population (ie: the cross number). In almost all cases this SHOULD NOT be the cultivar name. The only exception is in the case of outside material. You can attach other names (ie: the cultivar name) as synonyms during the properties step or Database References if they are accessible from a database such as CIAT.',
    '#required' => TRUE,
  );

  $type_options = tripal_cv_get_cvterm_options( variable_get('chado_stock_types_cv', 'null') );
  $type_options[0] = 'Select a Type';
  $form['create_parent']['type'] = array(
    '#type' => 'select',
    '#title' => t('Type of Germplasm'),
    '#options' => $type_options,
    '#description' => 'Choose the most accurate description of this germplasm. Additional details can be entered as properties.',
    '#required' => TRUE,
  );

    $synonym_prefix = '';
    if (preg_match('/[Ss]ummer/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'Sm'.$_SESSION['crossingblock_year'].'-P'; }
    if (preg_match('/[Ff]all/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'F'.$_SESSION['crossingblock_year'].'-P'; }
    if (preg_match('/[Ww]inter/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'W'.$_SESSION['crossingblock_year'].'-P'; }
    if (preg_match('/[Ss]pring/', $_SESSION['crossingblock_season'])) { $synonym_prefix = 'Sp'.$_SESSION['crossingblock_year'].'-P'; }
    $form['existing_parent']['synonym_prefix'] = array(
      '#type' => 'hidden',
      '#value' => $synonym_prefix
    );

  $form['create_parent']['parent_number'] = array(
    '#type' => 'textfield',
    '#title' => 'Parent Number',
    '#description' => 'This is a unique (to this crossing block) number to identify this parent',
    '#required' => TRUE,
  );

  $form['create_parent']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Create Germplasm and Assign as Parent',
  );

  return $form;
}

/**
 * Implements hook_form_submit()
 */
function create_stock_and_parent_alias_form_submit($form, $form_state) {
  module_load_include('inc', 'node', 'node.pages');
  global $user;

  //create main stock node by submitting form
  $node = array(
        'uid' => (string) $user->uid,
        'name' => (string) $user->name,
        'type' => 'chado_stock',
        'language' => '',
        'title' => $form_state['values']['name'],
        'uniquename' => $form_state['values']['name'],
        'type_id' => $form_state['values']['type'],
        'organism_id' => $_SESSION['organism_id'],
        'simulate_multipart' => FALSE,
  );

  $node_form_state = array(
      'values' => array(
        'type_id' => $form_state['values']['type'],
        'organism_id' => $_SESSION['organism_id'],
        'uniquename' => $form_state['values']['name'],
        'title' => $form_state['values']['name'],
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

  // Add Alias/Synonym (stock property)------------------------------------------------
  $synonym = $form_state['values']['synonym_prefix'].$form_state['values']['parent_number'];
  $insert_values = array(
    'stock_id' => $node->stock->stock_id,
    'type_id' => array(
      'name' => 'synonym',
      'cv_id' => variable_get('chado_stock_prop_types_cv', 'null')
    ),
    'value' => $synonym,
    'rank' => 1
  );
  $success = tripal_core_chado_insert('stockprop', $insert_values);
  if ($success) {
    drupal_set_message('Added synonym ('.$synonym.') to '.l($node->stock->name, 'node/'.$node->nid).' which designates it as a parent of the current crossing block');
  } else {
    drupal_set_message('Unable to Assign Parent', 'error');
  }
}
