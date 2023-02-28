<?php
/**
 * @class
 * Purpose: Supply a germplasm list for a stockcollection.
 *
 * Data:
 *   Details for each germplasm are stored in stock and stockprop. The
 *   connection between stockcollection and stocks is in stockcollection_stock.
 * Assumptions: None known.
 *
 * NOTE: This field will optionally display a country of origin column. In order
 *   to have it appear you must have the following term in the stock_property
 *   cv and use it in the stockprop table to indicate the country of origin for
 *   specific stocks.
 *    TERM: internal:country_of_origin (country_of_origin)
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
    'chado_table' => 'stockcollection',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => 'stockcollection_id',
    // The base table.
    'base_table' => 'stockcollection',
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
    // NOTE:
    // There are unpublished and published germplasm/stock records in chado.stock.
    // Widget (create), Formatter (view), Field Instance and Export sections
    // all use the function germcollection_get_germplasm() (@see API) to fetch 
    // list of germplasm. This function will exclude any unpublished germplasm 
    // therby making any list shown/generated in any of the sections mentioned 
    // consistent across.

    // Widget (create) is also equipped to prevent germplasm collection authors to
    // prevent adding germplasm that are unpublished type.

    // The Filter (outbound link) provided in this field might show unpublished 
    // germplasm added prior to this field potentially causing desimilar list.

    // We need the field name to ensure we save the data in the correct field!
    $field_name = $this->instance['field_name'];

    // Since the stock collection is the chado record...
    $stockcollection_id = $entity->chado_record_id;
    $results = germcollection_get_germplasm($stockcollection_id);
    
    $bundles = chado_query("SELECT tb.label, cb.chado_bundle_id
      FROM tripal_bundle tb LEFT JOIN chado_bundle cb ON cb.bundle_id=tb.id
      WHERE cb.data_table='stock'");
    
    foreach ($bundles as $r) {
      $table = 'chado_bio_data_'.$r->chado_bundle_id;
      $bundle_tables[] = $table;
      $bundle_labels[$table] = $r->label;
    }

    $entity->{$field_name}['und'] = [];
    
    if (count($results) > 0) {
      foreach ($results as $k => $stock) {
        list($name, $uniquename, $entity_id, $genus, $species, $origin, $stock_id) = $stock;

        // Save the chado data.
        $entity->{$field_name}['und'][$k]['chado-stockcollection__stock_id'] = $stock_id;
        $entity->{$field_name}['und'][$k]['chado-stockcollection__stockcollection_id'] = $stockcollection_id;

        // Save the value.
        $entity->{$field_name}['und'][$k]['value'] = [];
        $entity->{$field_name}['und'][$k]['value']['schema:name'] = $name;
        $entity->{$field_name}['und'][$k]['value']['data:identifier'] = $uniquename;
        $entity->{$field_name}['und'][$k]['value']['rdfs:type'] = $bundle_label;
        $entity->{$field_name}['und'][$k]['value']['taxrank:0000005'] = $genus;
        $entity->{$field_name}['und'][$k]['value']['taxrank:0000006'] = $species;
        $entity->{$field_name}['und'][$k]['value']['internal:country_of_origin'] = ($origin) ? $origin : '-'; 
        if (!empty($entity_id)) { $entity->{$field_name}['und'][$k]['value']['schema:url'] = 'bio_data/'. $entity_id; }
      }
    } 
    // @debug dpm($entity->{$field_name}['und'][0], 'first item from load');
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
