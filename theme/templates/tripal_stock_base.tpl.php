<?php
$stock = $node->stock;
$organism = $node->stock->organism_id;
$main_db_reference = $stock->dbxref_id;

// expand the text fields
$stock = chado_expand_var($stock, 'field', 'stock.description');
$stock = chado_expand_var($stock, 'field', 'stock.uniquename'); ?>

<div class="tripal_stock-data-block-desc tripal-data-block-desc"></div> <?php

// the $headers array is an array of fields to use as the colum headers.
// additional documentation can be found here
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
// This table for the stock has a vertical header (down the first column)
// so we do not provide headers here, but specify them in the $rows array below.
$headers = array();

// the $rows array contains an array of rows where each row is an array
// of values for each column of the table in that row.  Additional documentation
// can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
$rows = array();

// Organism
$organism = $stock->organism_id->genus ." " . $stock->organism_id->species ." (" . $stock->organism_id->common_name .")";
if (property_exists($stock->organism_id, 'nid')) {
  $organism = l("<i>" . $stock->organism_id->genus . " " . $stock->organism_id->species . "</i> (" . $stock->organism_id->common_name .")", "node/".$stock->organism_id->nid, array('html' => TRUE));
}
$rows[] = array(
  array(
    'data' => 'Scientific Name',
    'header' => TRUE
  ),
  $organism
);

// Stock Unique Name
$rows[] = array(
  array(
    'data' => 'Accession',
    'header' => TRUE,
  ),
  $stock->uniquename
);

// Stock Name
$rows[] = array(
  array(
    'data' => 'Name',
    'header' => TRUE,
    'width' => '20%',
  ),
  $stock->name
);

// Stock Type
$rows[] = array(
  array(
    'data' => 'Type',
    'header' => TRUE,
  ),
  ucwords(preg_replace('/_/', ' ', $stock->type_id->name))
);

// Maternal Parent
if (isset($node->germplasm->maternal_parent)) {
  if (isset($node->germplasm->maternal_parent->nid)) {
    $maternal_parent_name = l($node->germplasm->maternal_parent->stock->name, 'node/'.$node->germplasm->maternal_parent->nid);
  }
  else {
    $maternal_parent_name = $node->germplasm->maternal_parent->stock->name;
  }

  $rows[] = array(
    array(
      'data' => 'Maternal Parent',
      'header' => TRUE,
    ),
    $maternal_parent_name
  );
}

// Paternal Parent
if (isset($node->germplasm->paternal_parent)) {
  if (isset($node->germplasm->paternal_parent->nid)) {
    $paternal_parent_name = l($node->germplasm->paternal_parent->stock->name, 'node/'.$node->germplasm->paternal_parent->nid);
  }
  else {
    $paternal_parent_name = $node->germplasm->paternal_parent->stock->name;
  }

  $rows[] = array(
    array(
      'data' => 'Paternal Parent',
      'header' => TRUE,
    ),
    $paternal_parent_name
  );
}

// allow site admins to see the stock ID
if (user_access('administer tripal')) {
  // stock ID
  $rows[] = array(
    array(
      'data' => 'Stock ID',
      'header' => TRUE,
      'class' => 'tripal-site-admin-only-table-row',
    ),
    array(
      'data' => $stock->stock_id,
      'class' => 'tripal-site-admin-only-table-row',
    ),
  );
}
// Is Obsolete Row
if($stock->is_obsolete == TRUE){
  $rows[] = array(
    array(
      'data' => '<div class="tripal_stock-obsolete">This stock is obsolete</div>',
      'colspan' => 2
    ),
  );
}

// the $table array contains the headers and rows array as well as other
// options for controlling the display of the table.  Additional
// documentation can be found here:
// https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
$table = array(
  'header' => $headers,
  'rows' => $rows,
  'attributes' => array(
    'id' => 'tripal_stock-table-base',
    'class' => 'tripal-data-table'
  ),
  'sticky' => FALSE,
  'caption' => '',
  'colgroups' => array(),
  'empty' => '',
);

// once we have our table array structure defined, we call Drupal's theme_table()
// function to generate the table.
print theme_table($table);

// add in the description if there is one
if (property_exists($stock, 'description')) { ?>
  <div style="text-align: justify"><?php print $stock->description; ?></div> <?php
}