<?php

/**
 * Contains views integration code
 */

/**
 * Implements hook_views_data_alter().
 * Used to add Chado <-> Node Joins & Relationships
 *   since you need to add to the $data['node'] to do this
 *
 * @ingroup tripal_views
 */
function tripal_germplasm_views_data_alter(&$data) {

  $setup_id = tripal_get_lightest_views_integration_setup('stock');
  $base_table = 'stock';
  $linker_table = 'tripal_germplasm';
  $base_title = 'Tripal Germplasm';

  // add in joins to the node tables if the Chado schema is local
  if (tripal_core_chado_schema_exists()) {
    // if a node linking table exists then add in the joins
    if (db_table_exists($linker_table) AND $setup_id > 0) {

      // Adds in a chado base table => node relationship
      // This allows controlled joining to multiple nodes per line
      // Use Case:  link to feature and organism nodes on a feature listing
      $fake_field = 'tripal_germplasm_id';
      $data[$base_table][$fake_field] = array(
        'group' => $base_title,
        'title' => 'Stock => Tripal Germplasm',
        'help' => "Links stock to $base_title.",
        'relationship' => array(
          'handler' => 'views_handler_relationship',
          'title' => t("Stock => $base_title"),
          'label' => t("Stock => $base_title"),
          'real field' => 'stock_id',
          'base' => 'tripal_germplasm',
          'base field' => 'stock_id'
        ),
      );

      // Links from tripal_germplasm to node
      $base_field = $base_table . '_id';
      $data[$linker_table]['table'] = array(
        'group' => 'Tripal Germplasm',
        'title' => 'Tripal Germplasm',
        'help' => 'The drupal linker table linking Drupal germplasm to chado stocks'
      );
      $data[$linker_table]['nid'] = array(
        'group' => 'Tripal Germplasm',
        'title' => 'Tripal Germplasm to Node',
        'help' => "Links $base_title to it's node.",
        'relationship' => array(
          'handler' => 'views_handler_relationship',
          'title' => t("$base_title => Node"),
          'label' => t("$base_title => Node"),
          'base' => 'node',
          'base field' => 'nid'
        ),
      );

      // Add Chado fields to a node-based view
      // This will only be done with relationships
      $base_field = $base_table . '_id';
      $data['node'][$base_field] = array(
        'group' => $base_title,
        'title' => $base_title,
        'help' => "Links node to chado $base_title.",
        'relationship' => array(
          'handler' => 'views_handler_relationship',
          'title' => t("Node => $base_title"),
          'label' => t("Node => $base_title"),
          'real field' => 'nid',
          'base' => $linker_table,
          'base field' => 'nid'
        ),
      );
    }
  }

  return $data;
}
