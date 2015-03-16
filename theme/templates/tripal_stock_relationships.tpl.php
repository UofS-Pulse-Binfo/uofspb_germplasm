<?php
/* Typically in a Tripal template, the data needed is retrieved using a call to
 * chado_expand_var function.  For example, to retrieve all
 * of the stock relationships for this node, the following function call would be made:
 *
 *   $stock = chado_expand_var($stock,'table','stock_relationship');
 *
 * However, this function call can be extremely slow when there are numerous relationships.
 * This is because the chado_expand_var function is recursive and expands
 * all data following the foreign key relationships tree.  Therefore, to speed retrieval
 * of data, a special variable is provided to this template:
 *
 *   $stock->all_relationships;
 *
 * This variable is an array with two sub arrays with the keys 'object' and 'subject'.  The array with
 * key 'object' contains relationships where the stock is the object, and the array with
 * the key 'subject' contains relationships where the stock is the subject
 */
?>

<style>
.progeny .current-parent {
  color: #BABABA;
  font-style: italic;
}
</style>


<div class="tripal_stock-data-block-desc tripal-data-block-desc"></div>

<?php

  ///////////////////////////
  // Parents

  $headers = array();
  $rows = array();

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
        'width' => '20%',
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
        'width' => '20%',
      ),
      $paternal_parent_name
    );

  }

  if (!empty($rows)) {

    print '<h3 class="progeny">Parents</h3>';

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
    print theme_table($table);
  }

  ///////////////////////////
  // Progeny
  if (!empty($node->germplasm->progeny)) {?>

    <div class="progeny">
      <h3>Progeny</h3>
      <p class="description">The germplasm listed below are the progeny of <em><?php print $node->stock->name; ?></em> (shown in grey below):</p>
    <?php
    $headers = array('Progeny', 'Maternal Parent', 'Paternal Parent');
    $rows = array();

    foreach ($node->germplasm->progeny as $p) {

      $progeny_name = (isset($p['progeny']['nid'])) ? l($p['progeny']['name'], 'node/'.$p['progeny']['nid']) : $p['progeny']['name'];

      if ($p['maternal']['stock_id'] == $node->stock->stock_id) {
        $maternal_parent = '<span class="current-parent">' . $p['maternal']['name'] . '</span>';
      }
      else {
        $maternal_parent = (isset($p['maternal']['nid'])) ? l($p['maternal']['name'], 'node/'.$p['maternal']['nid']) : $p['maternal']['name'];
      }

      if ($p['paternal']['stock_id'] == $node->stock->stock_id) {
        $paternal_name = '<span class="current-parent">' . $p['paternal']['name'] . '</span>';
      }
      else {
        $paternal_name = (isset($p['paternal']['nid'])) ? l($p['paternal']['name'], 'node/'.$p['paternal']['nid']) : $p['paternal']['name'];
      }

      $rows[] = array(
        $progeny_name,
        $maternal_parent,
        $paternal_name
      );
    }


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
    print theme_table($table);?>

    </div>
  <?php }

  ///////////////////////////
  // Remaining Relationships
  $relationships = $node->stock->all_relationships;
  unset(
    $relationships['object'][$parent_rel_type['maternal']->title],
    $relationships['object'][$parent_rel_type['paternal']->title],
    $relationships['subject'][$parent_rel_type['maternal']->title],
    $relationships['subject'][$parent_rel_type['paternal']->title]
  );

  if (!empty($relationships['object']) OR !empty($relationships['subject'])) {?>

    <div class="non-parental">
      <h3>Additional Relationships</h3>

    <?php
    foreach (array('object', 'subject') as $side) {
      foreach($relationships[$side] as $type => $stock_types) {
        if ($side == 'object') {?>
          <p class="description">The following germplasm/stocks <strong><?php print $type; ?> <em><?php print $node->stock->name; ?></em></strong>:</p>
          <ul>
        <?php } else { ?>
          <p class="description"><em><strong><?php print $node->stock->name; ?></em> <?php print $type; ?></strong> of the following germplasm/stocks:</p>
          <ul>
        <?php }
        foreach ($stock_types as $records) {
          foreach($records as $record) {
            $record = $record->record;

            if ($side == 'object') {
              $name = (isset($record->subject_id->nid)) ? l($record->subject_id->name, 'node/'.$record->subject_id->nid) : $record->subject_id->name;
            }
            else {
              $name = (isset($record->object_id->nid)) ? l($record->object_id->name, 'node/'.$record->object_id->nid) : $record->object_id->name;
            }?>

            <li><?php print $name; ?></li>
            <?php
          }
        }
        print '</ul>';
      }
    }
  }
