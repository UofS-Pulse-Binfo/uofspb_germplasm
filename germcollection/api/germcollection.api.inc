<?php
/**
 * @file
 * Contains general API required by this field.
 * 
 * @param $collection_id
 *   Integer, collection id number.
 * 
 * @return array
 *   Germplasm collection where each item in the array is - Name, Accession, Entity ID, Genus, Species and Origin (Country of origin).
 */
function germcollection_get_germplasm($collection_id) {
  // Our researchers are very concerned with origin so we're going to add
  // that to this table if the term exists and more then 50% of germplasm
  // have the property.
  $origin_term = chado_get_cvterm(['name' => 'country_of_origin', 'cv_id' => ['name' => 'stock_property']]);

  $sql = "  
    SELECT 
      s.stock_id, s.name, s.uniquename, o.genus, o.species, s.type_id, 
      CASE WHEN prop.value IS NULL THEN 'n/a' ELSE prop.value END as origin
    FROM {stock} s LEFT JOIN {organism} o ON o.organism_id=s.organism_id %s
    WHERE %s s.stock_id IN (
      SELECT stock_id FROM {stockcollection_stock} WHERE stockcollection_id=:id
    )
    ORDER BY s.name ASC
  ";

  $clause = array('join' => '', 'where' => '');
  $args = array();
  
  if ($origin_term) {
    $clause['join'] = 'LEFT JOIN {stockprop} prop ON prop.stock_id=s.stock_id';
    $clause['where'] = 'prop.type_id = :origin_type AND';
    $args[':origin_type'] = $origin_term->cvterm_id;
  }
  
  $args[':id'] = $collection_id;

  $germplasm = chado_query(sprintf($sql, $clause['join'], $clause['where']), $args);
  $collection = array();

  if ($germplasm->rowCount() > 0) {
    foreach($germplasm as $i => $stock) {
      $entity_id = germcollection_get_entity_id($stock->stock_id, $stock->type_id);
      $collection[] = array(
        $stock->name,  // Stock name.
        $stock->uniquename, // Accession.
        $entity_id, // Entity id number.
        $stock->genus, // Genus.
        $stock->species, // Species.
        $stock->origin // Country of origin.
      );
    }
  }
  
  return $collection;
}

/**
 * Function get entity id from tripal 3 content type.
 *
 * @param $stock_id
 *   Integer, stock_id from chado.stock table.
 * @param $type_id
 *   Integer, type id of stock corresponding to a term in chado.cvterm.
 *
 * @return
 *   Entity id or # or an blank anchor when entity id cannot be extracted.
 */
function germcollection_get_entity_id($stock_id, $type_id) {
  // This character is also used to tell JS if node should be rendered as link or a text.
  $entity_id = '#';

  // Retrieves the name of the Tripal bundle based on the stock type_id
  $m = db_query('SELECT t2.name
    FROM {tripal_term} AS t1 INNER JOIN {tripal_bundle} AS t2 ON t1.id = t2.term_id
    WHERE t1.accession = (
      SELECT t3.accession
      FROM chado.dbxref AS t3 INNER JOIN chado.cvterm AS t4 USING(dbxref_id)
      WHERE t4.cvterm_id = :stock_type)', array(':stock_type' => $type_id))
    ->fetchField();

  if ($m) {
    $chado_bundle = 'chado_' . $m;

    $sql = sprintf('SELECT entity_id FROM %s WHERE stock_id = :stock_id LIMIT 1', $chado_bundle);
    $result = chado_query($sql, array(':stock_id' => $stock_id));

    if ($result->rowCount() > 0) {
      $entity_id = $result->fetchField();
    }
  }

  return $entity_id;
}

/**
 * Get experiment.
 * 
 * @param $experiment_id
 *   Integer, project_id number.
 * 
 * @return object
 *   Project row matching the experiment_id number.
 */
function germcollection_get_experiment($experiment_id) {
  $experiment = chado_query("SELECT * FROM {project} WHERE project_id = :experiment_id LIMIT 1",
    array(':experiment_id' => $experiment_id)
  );

  return ($experiment->rowCount() > 0) ? $experiment->fetchObject() : 0;
}