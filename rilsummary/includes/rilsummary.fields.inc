<?php
/**
 * @file
 * Contains all field specific code outside the classes.
 */

/**
 * Implements hook_bundle_fields_info().
 */
function rilsummary_bundle_fields_info($entity_type, $bundle) {
   $info = [];

  // Make sure this bundle is an RIL (CO_010:414 inbred line)...
  $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
  if ($term->name == '414 inbred line') {

    tripal_insert_cvterm(array(
      'id' => 'local:germ_ril_summary',
      'name' => 'germ_ril_summary',
      'cv_name' => 'tripal',
      'definition' => 'Summarizes RILs.',
    ));

    $field_type = $field_name = 'local__germ_ril_summary';
    $info[$field_name] = [
      'field_name' => $field_name,
      'type' => $field_type,
      'cardinality' => 1,
      'locked' => FALSE,
      'storage' => [
        'type' => 'field_chado_storage',
      ],
      'settings' => [],
    ];
 }

  return $info;
}


/**
 * Implements hook_bundle_instances_info().
 */
function rilsummary_bundle_instances_info($entity_type, $bundle) {
  $info = [];

  // Make sure this bundle is an RIL (CO_010:414 inbred line)...
  $term = tripal_load_term_entity(array('term_id' => $bundle->term_id));
  if ($term->name == '414 inbred line') {

    $field_name = 'local__germ_ril_summary';
    $info[$field_name] =  [
      'field_name' => $field_name,
      'entity_type' => $entity_type,
      'bundle' => $bundle->name,
      'label' => 'Germplasm RIL Summary',
      'description' => 'Germplasm RIL Summary',
      'required' => FALSE,
      'settings' => [
        'auto_attach' => TRUE,
        'chado_table' => 'stockprop',
        'chado_column' => 'stock_id',
        'base_table' => 'stock',
        'term_accession' => 'germ_ril_summary',
        'term_vocabulary' => 'local',
        'term_name' => 'germ ril summary',
      ],
      'widget' => [
        'type' => 'local__germ_ril_summary_widget',
        'settings' => [
          'display_label' => 1,
        ],
      ],
      'display' => [
        'default' => [
          'label' => 'inline',
          'type' => 'local_germ_ril_summary_formatter',
          'settings' => [],
        ],
      ],
    ];
  }

  return $info;
}
