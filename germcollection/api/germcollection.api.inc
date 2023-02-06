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
        $stock->origin, // Country of origin.
        $stock->stock_id // Stock Id number.
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

    $sql = sprintf('SELECT entity_id FROM %s WHERE record_id = :stock_id LIMIT 1', $chado_bundle);
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

/**
 * Get genus + species (scientific name)
 * 
 * @return array
 *   Associative array where the key is organism id number and value
 *   is combination of genus and species.
 */
function germcollection_get_scientific_name() {
  $names = array();
  
  // Query names in chado.organism.
  $sql = "SELECT organism_id, CONCAT(genus, ' ', species) FROM {organism} ORDER BY genus ASC";
  $query = chado_query($sql);

  if ($query->rowCount() > 0) {
    $names = $query->fetchAllKeyed();
  }

  return $names;
}

/**
 * Get stock collection information.
 * 
 * @param id
 *   Integer, stock collection id number. 
 * 
 * @return array
 *   Content type data row of the matching stock collection (id, name, shortname)
 */
function germcollection_get_collection($id) {  
  // Query stock collection in chado.stockcollection.
  $sql = "SELECT stockcollection_id, name, uniquename 
    FROM {stockcollection} WHERE stockcollection_id = :id LIMIT 1";
  $collection = chado_query($sql, array(':id' => $id));

  // stockcollection: stockcollection_id, name, uniquename, type_id.
  return ($collection->rowCount() > 0) ? $collection->fetchObject() : 0;
}

/**
 * Get stock by name+uniquename+organism.
 * 
 * @param $uniquename
 *   String, uniquename of germplasm - uniquename field of chado.stock table.
 * @param $name
 *   String, name of germplasm - name field of chado.stock table.
 * @param $organism
 *   String, combination of genus and species of germplasm.
 *   ie. Lens culinaris.
 *
 * @return integer
 *   Stock id number of the matching germplasm.
 */
function germcollection_get_stock_id($uniquename, $name, $organism) {
  $sql = "
    SELECT stock_id FROM {stock} AS t1 INNER JOIN {organism} AS t2 USING (organism_id) 
    WHERE 
      t1.uniquename = :uniquename AND 
      t1.name = :name AND 
      CONCAT(t2.genus, ' ', t2.species) = :organism
    LIMIT 1
  ";

  $stock = chado_query($sql, array(
    ':uniquename' => trim($uniquename),
    ':name' => trim($name),
    ':organism' => trim($organism)
  ));

  return ($stock->rowCount() > 0) ? $stock->fetchField() : 0;
}