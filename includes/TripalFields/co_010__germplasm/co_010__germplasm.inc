<?php
/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class co_010__germplasm extends ChadoField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Germplasm List';

  // The default description for this field.
  public static $default_description = 'A listing of germplasm in a stock collection.';

  // The default widget for this field.
  public static $default_widget = 'co_010__germplasm_widget';

  // The default formatter for this field.
  public static $default_formatter = 'co_010__germplasm_formatter';

  // The module that manages this field.
  // If no module manages the field (IE it's added via libraries)
  // set this to 'tripal_chado'
  public static $module = 'germcollection';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = array(
    'storage' => 'field_chado_storage',
     // It is expected that all fields set a 'value' in the load() function.
     // In many cases, the value may be an associative array of key/value pairs.
     // In order for Tripal to provide context for all data, the keys should
     // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
     // function that are supported by the query() function should be
     // listed here.
     'searchable_keys' => array(),
  );

  // Indicates the download formats for this field.  The list must be the
  // name of a child class of the TripalFieldDownloader.
  public static $download_formatters = array(
     'TripalTabDownloader',
     'TripalCSVDownloader',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = array(
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'CO_010',
    // The name of the term.
    'term_name' => 'germplasm',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000251',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => '',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => '',
    // The base table.
    'base_table' => '',
  );

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick search field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = FALSE;

   /**
    * @see ChadoField::load()
    *
    **/

  public function load($entity) {

    // We need the field name to ensure we save the data in the correct field!
    $field_name = $this->instance['field_name'];

    // Since the stock collection is the chado record...
    $stockcollection_id = $entity->chado_record_id;

    $bundles = chado_query("SELECT tb.label, cb.chado_bundle_id FROM tripal_bundle tb LEFT JOIN chado_bundle cb ON cb.bundle_id=tb.id WHERE cb.data_table='stock'");
    foreach ($bundles as $r) {
      $table = 'chado_bio_data_'.$r->chado_bundle_id;
      $bundle_tables[] = $table;
      $bundle_labels[$table] = $r->label;
    }

    // Our researchers are very concerned with origin so we're going to add
    // that to this table if the term exists and more then 50% of germplasm
    // have the property.
    $origin_term = chado_get_cvterm(array('name' => 'country_of_origin', 'cv_id' => array('name' => 'stock_property'))); 
    $origin_accession = FALSE;

    if ($origin_term) {
      $origin_accession = 'internal:country_of_origin';
      $results = chado_query('
        SELECT s.stock_id, s.name, s.uniquename, o.genus, o.species, prop.value as origin
        FROM {stock} s 
        LEFT JOIN {organism} o ON o.organism_id=s.organism_id
        LEFT JOIN {stockprop} prop ON prop.stock_id=s.stock_id 
        WHERE
          prop.type_id = :origin_type AND
          s.stock_id IN (SELECT stock_id FROM {stockcollection_stock} WHERE stockcollection_id=:id)',
        array(':id' => $stockcollection_id, ':origin_type' => $origin_term->cvterm_id));
    }
    else {
      $results = chado_query('
        SELECT s.stock_id, s.name, s.uniquename, o.genus, o.species
        FROM {stock} s 
        LEFT JOIN {organism} o ON o.organism_id=s.organism_id
        WHERE s.stock_id IN (SELECT stock_id FROM {stockcollection_stock} WHERE stockcollection_id=:id)',
        array(':id' => $stockcollection_id));
    }
    foreach ($results as $k => $r) {

      // Entity ID
      foreach ($bundle_tables as $bundle_table) {
        $entity_id = db_query("SELECT entity_id FROM $bundle_table WHERE record_id=:id", 
          array(':id' => $r->stock_id))->fetchField();
        $bundle_label = $bundle_labels[$bundle_table];
        if ($entity_id) { break; }
      }

      // Save the chado data.
      $entity->{$field_name}['und'][$k]['chado-stockcollection__stock_id'] = $r->stock_id;
      $entity->{$field_name}['und'][$k]['chado-stockcollection__stockcollection_id'] = $stockcollection_id;

      // Save the value.
      $entity->{$field_name}['und'][$k]['value'] = array();
      $entity->{$field_name}['und'][$k]['value']['schema:name'] = $r->name;
      $entity->{$field_name}['und'][$k]['value']['data:identifier'] = $r->uniquename;
      $entity->{$field_name}['und'][$k]['value']['rdfs:type'] = $bundle_label;
      $entity->{$field_name}['und'][$k]['value']['taxrank:0000005'] = $r->genus;
      $entity->{$field_name}['und'][$k]['value']['taxrank:0000006'] = $r->species;
      if ($origin_accession) { $entity->{$field_name}['und'][$k]['value'][$origin_accession] = $r->origin; }
      if (!empty($entity_id)) { $entity->{$field_name}['und'][$k]['value']['schema:url'] = 'bio_data/'.$entity_id; }
    }
    // @debug dpm($entity->{$field_name}['und'][0], 'first item from load');
  }



    /**
    * @see ChadoField::query()
    *
    **/

  public function query($query, $condition) {
  }

    /**
    * @see ChadoField::queryOrder()
    *
    **/

  public function queryOrder($query, $order) {
  }


    /**
    * @see ChadoField::elementInfo()
    *
    **/
    
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return array(
      $field_term => array(
        'operations' => array('eq', 'ne', 'contains', 'starts'),
        'sortable' => TRUE,
        'searchable' => TRUE,
      ),
    );
  }

}


