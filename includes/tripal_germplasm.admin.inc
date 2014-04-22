<?php
/**
 * @file
 * Administration for Germplasm
 */

/**
 * Admin Launchpad
 */
function tripal_germplasm_admin_germplasm_listing() {
  $output = '';

  return $output;
}

/**
 * Purpose: Display Administrative Options
 *
 * @return HTML markup of the administrative options
 */
function tripal_germplasm_admin_settings_form($form, $form_state) {

	$form['prefix'] = array(
		'#type' => 'fieldset',
		'#title' => t('Germplasm Prefix'),
	);

	$form['prefix']['germplasm_prefix'] = array(
		'#type' => 'textfield',
		'#required' => TRUE,
		'#title' => t('Germplasm Prefix'),
		'#description' => t('The uniquename of any germplasm created through one of'
											.' the germplasm management forms is composed of the'
											.' Germplasm Prefix, which is set here, and the stock_id.'
											.' For example, if you leave the default as is then stock'
											.' #1338 will have a uniquename of GERM:1338.'),
		'#default_value' => variable_get('germplasm_prefix', 'GERM:'),
	);

	$form['prefix']['dbxref'] = array(
	  '#type' => 'prefix',
	  '#title' => t('Add Database Reference for all germplasm created'),
	  '#description' => 'All germplasm created through this site will have a main database reference which is an accession with the same form as the uniquename.',
	);

  $db_options = tripal_db_get_db_options();
  $db_options[0] = 'Select a Database';
	$form['prefix']['dbxref']['db'] = array(
    '#type' => 'select',
		'#required' => TRUE,
    '#title' => t('Database'),
    '#options' => $db_options,
    '#default_value' => variable_get('tripal_stock_main_dbxref_db', 0),
    '#description' => 'The database selected here will contain the accessions for all germplasm created through this site.',
	);

	$form['prefix']['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Change Prefix',
		'#validate' => array('tripal_germplasm_admin_germplasm_prefix_form_validate'),
 		'#submit' => array('tripal_germplasm_admin_germplasm_prefix_form_submit')
	);

	$form['cvs'] = array(
	  '#type' => 'fieldset',
	  '#title' => 'Germplasm Controlled Vocabularies'
	);

	$cvs = tripal_get_cv_select_options();
	$cvs[0] = '- Select -';

	$germplasm_type_cv_id = variable_get('germplasm_type_cv_id', 0);
	$germplasm_rel_type_cv_id = variable_get('germplasm_rel_type_cv_id', 0);

	$form['cvs']['stock_type'] = array(
	  '#type' => 'select',
		'#required' => TRUE,
	  '#title' => 'Germplasm Type',
	  '#description' => 'The controlled vocabulary to use for the "Type" drop-down when creating germplasm.',
	  '#options' => $cvs,
	  '#default_value' => $germplasm_type_cv_id
	);

	$form['cvs']['relationship_type'] = array(
	  '#type' => 'select',
		'#required' => TRUE,
	  '#title' => 'Germplasm Relationship Type',
	  '#description' => 'The controlled vocabulary to use for the Germplasm Relationships "Type" drop-down when creating germplasm.',
	  '#options' => $cvs,
	  '#default_value' => $germplasm_rel_type_cv_id,
	);

	$form['cvs']['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Set Vocabularies',
		'#validate' => array('tripal_germplasm_admin_germplasm_cvs_form_validate'),
 		'#submit' => array('tripal_germplasm_admin_germplasm_cvs_form_submit')
	);

	$form['parent'] = array(
	  '#type' => 'fieldset',
	  '#title' => 'Germplasm Parents'
	);

  if ($germplasm_rel_type_cv_id > 0) {
    $cvterms = chado_select_record('cvterm', array('cvterm_id','name'),array('cv_id' => $germplasm_rel_type_cv_id));
    foreach($cvterms as $cvt) {
      $rel_types[ $cvt->cvterm_id ] = $cvt->name;
    }
  }
  else {
    $rel_types = array(0 => '- Select -');
  }

	$form['parent']['maternal_parent_type_id'] = array(
	  '#type' => 'select',
	  '#title' => 'Maternal Parent Relationship Type',
	  '#description' => 'The relationship type to use when specifying the maternal parent relationship for germplasm.',
	  '#options' => $rel_types,
	  '#default_value' => variable_get('germplasm_mparent_rel_type_id', NULL),
		'#required' => TRUE,
		'#disabled' => ($germplasm_rel_type_cv_id > 0) ? FALSE : TRUE,
	);

	$form['parent']['paternal_parent_type_id'] = array(
	  '#type' => 'select',
	  '#title' => 'Paternal Parent Relationship Type',
	  '#description' => 'The relationship type to use when specifying the paternal parent relationship for germplasm.',
	  '#options' => $rel_types,
	  '#default_value' => variable_get('germplasm_pparent_rel_type_id', NULL),
		'#required' => TRUE,
		'#disabled' => ($germplasm_rel_type_cv_id > 0) ? FALSE : TRUE,
	);

	$form['parent']['submit'] = array(
		'#type' => 'submit',
		'#value' => 'Set Parent Relationship Types',
		'#validate' => array('tripal_germplasm_admin_germplasm_parent_type_form_validate'),
 		'#submit' => array('tripal_germplasm_admin_germplasm_parent_type_form_submit')
	);
/**
	$form['cross_db'] = array(
		'#type' => 'fieldset',
		'#title' => t('Cross Databases'),
		'#description' => t('All Cross Numbers are stored as database references.'
											 .' This ensures that all cross numbers are linked together'
											 .' and their source recorded. It\'s assumed but not ensured'
											 .' that all cross numbers within a given database are unique.'
											 .' Check the box beside any databases listed below that are'
											 .' used to store cross numbers. This selection will be used'
											 .' to populate any select box allowing users to select the'
											 .' cross number database.'),
	);

 	$form['cross_db']['cross_databases'] = array(
 		'#type' => 'checkboxes',
 		'#title' => t('Available Databases'),
 		'#options' => tripal_db_get_db_options(),
 		'#default_value' => array_keys(variable_get( 'germplasm_cross_dbs', array() )),
 	);

 	$form['cross_db']['submit'] = array(
 		'#type' => 'submit',
 		'#value' => 'Set Cross Databases',
 		'#validate' => array('tripal_germplasm_admin_cross_databases_form_validate'),
 		'#submit' => array('tripal_germplasm_admin_cross_databases_form_submit')
 	);
*/
	return $form;
}

/**
 * Implements hook_form_validate().
 * Pupose: To change the germplasm prefix
 */
function tripal_germplasm_admin_germplasm_prefix_form_validate($form, $form_state) {

}

/**
 * Implements hook_form_submit().
 * Pupose: To change the germplasm prefix
 */
function tripal_germplasm_admin_germplasm_prefix_form_submit($form, $form_state) {

	variable_set('germplasm_prefix', $form_state['values']['germplasm_prefix']);
	variable_set('tripal_stock_main_dbxref_db', $form_state['values']['db']);

}

/**
 * Implements hook_form_validate().
 * Pupose: provide admin with the ability to set cvs
 */
function tripal_germplasm_admin_germplasm_cvs_form_validate($form, $form_state) {

}

/**
 * Implements hook_form_validate().
 * Pupose: provide admin with the ability to set cvs
 */
function tripal_germplasm_admin_germplasm_cvs_form_submit($form, $form_state) {

	variable_set('germplasm_type_cv_id', $form_state['values']['stock_type']);
	variable_set('germplasm_rel_type_cv_id', $form_state['values']['relationship_type']);

}

/**
 * Implements hook_form_validate().
 * Pupose: provide admin with the ability to set parent relationship types
 */
function tripal_germplasm_admin_germplasm_parent_type_form_validate($form, $form_state) {

}

/**
 * Implements hook_form_validate().
 * Pupose: provide admin with the ability to set parent relationship types
 */
function tripal_germplasm_admin_germplasm_parent_type_form_submit($form, $form_state) {

	variable_set('germplasm_mparent_rel_type_id', $form_state['values']['maternal_parent_type_id']);
	variable_set('germplasm_pparent_rel_type_id', $form_state['values']['paternal_parent_type_id']);

}

/**
 * Implements hook_form_validate().
 * Pupose: provide admin with the ability to indicate which databases
 *   are cross number databases
 */
function tripal_germplasm_admin_cross_databases_form_validate($form, $form_state) {

}

/**
 * Implements hook_form_submit().
 * Pupose: provide admin with the ability to indicate which databases
 *   are cross number databases
 */
function tripal_germplasm_admin_cross_databases_form_submit($form, $form_state) {
	$cross_dbs = array();
	foreach ($form_state['values']['cross_databases'] as $k => $v) {
		if ($k == $v) {
			$db = tripal_db_get_db_by_db_id($v);
			$cross_dbs[$db->db_id] = $db->name;
		}
	}

	variable_set('germplasm_cross_dbs', $cross_dbs);
}