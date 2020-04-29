<?php
/**
 * @class
 * Purpose:
 *
 * Data:
 * Assumptions:
 */
class local__project_germcollection extends ChadoField {

  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Project-Related Germplasm Collection';

  // The default description for this field.
  public static $default_description = 'Lists germplasm related to a project.';

  // The default widget for this field.
  public static $default_widget = 'local__project_germcollection_widget';

  // The default formatter for this field.
  public static $default_formatter = 'local__project_germcollection_formatter';

  // The module that manages this field.
  // If no module manages the field (IE it's added via libraries)
  // set this to 'tripal_chado'
  public static $module = 'germcollection';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = [
    'storage' => 'field_chado_storage',
     // It is expected that all fields set a 'value' in the load() function.
     // In many cases, the value may be an associative array of key/value pairs.
     // In order for Tripal to provide context for all data, the keys should
     // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
     // function that are supported by the query() function should be
     // listed here.
     'searchable_keys' => [],
  ];

  // Indicates the download formats for this field.  The list must be the
  // name of a child class of the TripalFieldDownloader.
  public static $download_formatters = [
     'TripalTabDownloader',
     'TripalCSVDownloader',
  ];

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'project_germcollection',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'project_germcollection',
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
    'chado_table' => 'project_stockcollection',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => 'project_sotckcollection_id',
    // The base table.
    'base_table' => 'project',
  ];

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
   */
  public function load($entity) {

    // We need the field name to ensure we save the data in the correct field!
    $field_name = $this->instance['field_name'];

    // Since the project is the chado record...
    $project_id = $entity->chado_record_id;

    $bundle_id = chado_query("SELECT chado_bundle_id FROM chado_bundle WHERE data_table='stockcollection'")->fetchField();
    $bundle_table = 'chado_bio_data_'.$bundle_id;

    $results = chado_query('
      SELECT psc.project_stockcollection_id, sc.stockcollection_id,
        sc.name, sc.uniquename, cvt.name as type_name
      FROM {stockcollection} sc
      LEFT JOIN {project_stockcollection} psc ON psc.stockcollection_id=sc.stockcollection_id
      LEFT JOIN {cvterm} cvt ON cvt.cvterm_id=sc.type_id
      WHERE psc.project_id=:id',
        [':id' => $project_id]);
    $entity->{$field_name}['und'] = [];
    foreach ($results as $k => $r) {

      // Retrieve entity_id for current stockcollection.
      $entity_id= NULL;
      if ($bundle_id) {
        $entity_id = db_query("SELECT entity_id FROM $bundle_table WHERE record_id=:id",
          [':id' => $r->stockcollection_id])->fetchField();
      }

      // Save the chado data.
      $entity->{$field_name}['und'][$k]['chado-project__project_id'] = $project_id;
      $entity->{$field_name}['und'][$k]['chado-project_stockcollection__project_id'] = $project_id;
      $entity->{$field_name}['und'][$k]['chado-project_stockcollection__stockcollection_id'] = $r->stockcollection_id;
      $entity->{$field_name}['und'][$k]['chado-project_stockcollection__project_stockcollection_id'] = $r->project_stockcollection_id;

      // Save the value.
      $entity->{$field_name}['und'][$k]['value'] = [];
      $entity->{$field_name}['und'][$k]['value']['schema:name'] = $r->name;
      $entity->{$field_name}['und'][$k]['value']['data:identifier'] = $r->uniquename;
      $entity->{$field_name}['und'][$k]['value']['schema:additional_type'] = $r->type_name;
      if (!empty($entity_id)) { $entity->{$field_name}['und'][$k]['value']['schema:url'] = 'bio_data/'.$entity_id; }
    }
  }



  /**
   * @see ChadoField::query()
   *
   */
  public function query($query, $condition) {
  }

  /**
   * @see ChadoField::queryOrder()
   *
   */
  public function queryOrder($query, $order) {
  }


  /**
   * @see ChadoField::elementInfo()
   *
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return [
      $field_term => [
        'operations' => ['eq', 'ne', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
      ],
    ];
  }

}
