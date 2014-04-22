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

  // 1) if we are editing an existing node then the stock is already part of the node
  if (property_exists($node, 'stock')) {
    $sname = $node->stock->name;
    $stock_id = $node->stock->stock_id;
    $type_id = $node->stock->type_id->cvterm_id;
    $organism_id = $node->stock->organism_id->organism_id;
  }

  // 2) if we are re constructing the form from a failed validation or ajax callback
  // then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
    $sname = $form_state['values']['sname'];
    $stock_id = $form_state['values']['stock_id'];
    $type_id = $form_state['values']['type_id'];
    $organism_id = $form_state['values']['organism_id'];
  }

  // 3) if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
    $sname = $form_state['input']['sname'];
    $stock_id = $form_state['input']['stock_id'];
    $type_id = $form_state['input']['type_id'];
    $organism_id = $form_state['input']['organism_id'];
  }

  $form['core'] = array(
    '#type' => 'fieldset',
  );

  $form['core']['name'] = array(
    '#type' => 'textfield',
    '#title' => 'Name',
    '#description' => 'The name of the germplasm.',
    '#required' => TRUE
  );

  $organism_options = tripal_get_organism_select_options();
  $form['core']['organism_id'] = array(
    '#type' => 'select',
    '#title' => 'Scientific Name',
    '#description' => 'The genus and species of this germplasm.',
    '#options' => $organism_options,
    '#required' => TRUE
  );

  $type_options = tripal_get_germplasm_type_select_options();
  $form['core']['type_id'] = array(
    '#type' => 'select',
    '#title' => 'Type',
    '#description' => 'The type best matching this germplasm.',
    '#options' => $type_options,
    '#required' => TRUE
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
    'cv_id' => tripal_get_germplasm_relationship_type_cv_id()
  );

  // Finally, and add the additional form elements to the form
  chado_add_node_form_relationships($form, $form_state, $details);
  $form['relationships']['#weight'] = 1;

  $form['relationships']['maternal_parent'] = array(
    '#type' => 'textfield',
    '#title' => 'Maternal Parent',
    '#description' => 'The name of the maternal parent of this germplasm.',
    '#weight' => -10
  );

  $form['relationships']['paternal_parent'] = array(
    '#type' => 'textfield',
    '#title' => 'Paternal Parent',
    '#description' => 'The name of the paternal parent of this germplasm.',
    '#weight' => -10
  );

/**
  // Cross
  //-------------------------
  $form['cross'] = array(
    '#type' => 'fieldset',
    '#title' => 'Cross Details',
    '#weight' => 2,
    '#collapsible' => TRUE,
    '#collapsed' => !tripal_is_germplasm_cross(array('type_id' => $type_id))
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

  // RIL
  //-------------------------
  $form['ril'] = array(
    '#type' => 'fieldset',
    '#title' => 'RIL Details',
    '#weight' => 4,
    '#collapsible' => TRUE,
    '#collapsed' => !tripal_is_germplasm_ril(array('type_id' => $type_id))
  );

  // Accession
  //-------------------------
  $form['accession'] = array(
    '#type' => 'fieldset',
    '#title' => 'Accession Details',
    '#weight' => 5,
    '#collapsible' => TRUE,
    '#collapsed' => !tripal_is_germplasm_accession(array('type_id' => $type_id))
  );
*/

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

}
