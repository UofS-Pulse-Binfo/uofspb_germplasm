<?php
/**
 * @class
 * Purpose: Provide quick browse functionality for entity pages
 *
 * Data: No data.
 * Assumptions:
 */
class local__germ_ril_summary extends TripalField {
  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendant class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------
  // The default label for this field.
  public static $default_label = 'Germplasm RIL Summary';
  // The default description for this field.
  public static $default_description = 'Germplasm RIL Summary';
  // The default widget for this field.
  public static $default_widget = 'local__germ_ril_summary_widget';
  // The default formatter for this field.
  public static $default_formatter = 'local__germ_ril_summary_formatter';
  // The module that manages this field.
  public static $module = 'germ_summary';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = array(
    'storage' => 'tripal_no_storage',
    // It is expected that all fields set a 'value' in the load() function.
    // In many cases, the value may be an associative array of key/value pairs.
    // In order for Tripal to provide context for all data, the keys should
    // be a controlled vocabulary term (e.g. rdfs:type). Keys in the load()
    // function that are supported by the query() function should be
    // listed here.
    'browseable_keys' => array(),
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = array(
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'germ ril summary',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'germ_ril_summary',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table where the options for this specific field are stored.
    // This can be one of trpfancy_browse_options or trpfancy_browse_options_per_entity
    // based on admin configuration. Default: trpfancy_browse_options.
    'option_storage' => '',
    // A list of browser types this field intends to provide.
    'browser_types' => '',
  );

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;
  // A boolean specifying that the field will not contain any data. This
  // should exclude the field from web services or downloads.  An example
  // could be a quick browse field that appears on the page that redirects
  // the user but otherwise provides no data.
  public static $no_data = TRUE;

 /**
   * Loads the field values from the underlying data store.
   *
   * @param $entity
   *
   * @return
   *   An array of the following format:
   *     $entity->{$field_name}['und'][0]['value'] = $value;
   *   where:
   *     - $entity is the entity object to which this field is attached.
   *     - $field_name is the name of this field
   *     - 'und' is the language code (in this case 'und' == undefined)
   *     - 0 is the cardinality.  Increment by 1 when more than one item is
   *       available.
   *     - 'value' is the key indicating the value of this field. It should
   *       always be set.  The value of the 'value' key will be the contents
   *       used for web services and for downloadable content.  The value
   *       should be of the follow format types: 1) A single value (text,
   *       numeric, etc.) 2) An array of key value pair. 3) If multiple entries
   *       then cardinality should incremented and format types 1 and 2 should
   *       be used for each item.
   *   The array may contain as many other keys at the same level as 'value'
   *   but those keys are for internal field use and are not considered the
   *   value of the field.
   *
   *
   */
  public function load($entity) {
    // About the stock.
    $germ_ril_summary = array(
      'type' => $entity->chado_record->type_id->name,
      'name'   => $entity->title,
      'uniquename' => $entity->chado_record->uniquename,
      'maternal' => '',
      'mom_entity_id' => '',
      'paternal' => '',
      'dad_entity_id' => '',
      'generations' => array(),
      'complete' => '',
    );

    // Ontology terms required prepared by the summary component in hook_install of this module.
    // F generations.
    $gen = unserialize(variable_get('gs_var_fgens'));

    if ($gen) {
      $args = array();
      $args[':F1'] = $gen['F1']; $args[':F2'] = $gen['F2']; $args[':F3'] = $gen['F3']; $args[':F4'] = $gen['F4'];
      $args[':F5'] = $gen['F5']; $args[':F6'] = $gen['F6']; $args[':F7'] = $gen['F7']; $args[':F8'] = $gen['F8'];
      $args[':RIL_complete'] = $gen['RIL_complete'];

      $args[':is_mom'] = variable_get('gs_var_ismom');
      $args[':is_dad'] = variable_get('gs_var_isdad');
      $args[':is_sel'] = variable_get('gs_var_issel');

      $args[':stock_id'] = $entity->chado_record_id;


      // Fs and Parents.
      $f = chado_query("
        SELECT
            s.stock_id,
            mom.name AS maternal_parent, mom.type_id AS mom_type,
            dad.name AS paternal_parent, dad.type_id AS dad_type,

            STRING_AGG(
              CASE
                WHEN prop.type_id = :F1 THEN CONCAT('F1=>', TRIM(prop.value))
                WHEN prop.type_id = :F2 THEN CONCAT('F2=>', TRIM(prop.value))
                WHEN prop.type_id = :F3 THEN CONCAT('F3=>', TRIM(prop.value))
                WHEN prop.type_id = :F4 THEN CONCAT('F4=>', TRIM(prop.value))
                WHEN prop.type_id = :F5 THEN CONCAT('F5=>', TRIM(prop.value))
                WHEN prop.type_id = :F6 THEN CONCAT('F6=>', TRIM(prop.value))
                WHEN prop.type_id = :F7 THEN CONCAT('F7=>', TRIM(prop.value))
                WHEN prop.type_id = :F8 THEN CONCAT('F8=>', TRIM(prop.value))
                ELSE ''
              END, ' # ') AS property_generation,
            STRING_AGG(CASE WHEN prop.type_id = :RIL_complete THEN TRIM( prop.value ) END, '') AS property_complete
          FROM {stock} s
            LEFT JOIN {organism} o ON o.organism_id=s.organism_id

            LEFT JOIN {stock_relationship} origcross ON origcross.subject_id=s.stock_id
              AND origcross.type_id = :is_sel
            LEFT JOIN {stock_relationship} momr ON momr.object_id=origcross.object_id
              AND momr.type_id=:is_mom
            LEFT JOIN {stock_relationship} dadr ON dadr.object_id=origcross.object_id
              AND dadr.type_id=:is_dad

            LEFT JOIN {stock} mom ON momr.subject_id=mom.stock_id
            LEFT JOIN {stock} dad ON dadr.subject_id=dad.stock_id
            LEFT JOIN {organism} momo ON momo.organism_id=mom.organism_id
            LEFT JOIN {organism} dado ON dado.organism_id=dad.organism_id

            LEFT JOIN {stockprop} AS prop ON s.stock_id = prop.stock_id
          WHERE s.stock_id  = :stock_id
          GROUP BY
            s.stock_id, mom.name, mom.type_id, dad.name, dad.type_id", $args
        )
      ->fetchObject();

      if ($f) {
        // Debug values:
        // $germ_ril_summary['all'] = $f;

        $germ_ril_summary['maternal']    = $f->maternal_parent;
        $germ_ril_summary['mom_entity_id'] = grs_get_stock_entity_id($f->mom_id, $f->mom_type);
        $germ_ril_summary['paternal']    = $f->paternal_parent;
        $germ_ril_summary['dad_entity_id'] = grs_get_stock_entity_id($f->dad_id, $f->dad_type);
        $germ_ril_summary['complete']    = $f->property_complete;
        $germ_ril_summary['generations'] = $f->property_generation;
      }
    }

    // We need the field name to ensure we save the data in the correct field!
    $field_name = $this->instance['field_name'];
    $entity->{$field_name}['und'][0]['value']['local:germ ril summary'] = $germ_ril_summary;
  }

  /**
   * Provides a form for the 'Field Settings' of an instance of this field.
   *
   * This function corresponds to the hook_field_instance_settings_form()
   * function of the Drupal Field API.
   *
   * Validation of the instance settings form is not supported by Drupal, but
   * the TripalField class does provide a mechanism for supporting validation.
   * To allow for validation of your setting form you must call the parent
   * in your child class:
   *
   * @code
   *   $element = parent::instanceSettingsForm();
   * @endcode
   *
   * Please note, the form generated with this function does not easily
   * support AJAX calls in the same way that other Drupal forms do.  If you
   * need to use AJAX you must manually alter the $form in your ajax call.
   * The typical way to handle updating the form via an AJAX call is to make
   * the changes in the form function itself but that doesn't work here.
   */
  public function instanceSettingsForm() {

    // Retrieve the current settings.
    // If this field was just created these will contain the default values.
    $settings = $this->instance['settings'];

    // Allow the parent Tripal Field to set up the form element for us.
    $element = parent::instanceSettingsForm();

    return $element;
  }
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
function grs_get_stock_entity_id($stock_id, $type_id) {
  // This character is also used to tell JS if node should be rendered as link or a text.
  $entity_id = '#';

  $m = db_query('SELECT t2.name
    FROM {tripal_term} AS t1 INNER JOIN {tripal_bundle} AS t2 ON t1.id = t2.term_id
    WHERE t1.accession = (
      SELECT t3.accession
      FROM chado.dbxref AS t3 INNER JOIN chado.cvterm AS t4 USING(dbxref_id)
      WHERE t4.cvterm_id = :stock_type)', array(':stock_type' => $type_id))
    ->fetchField();

  if ($m) {
    $chado_bundle = 'chado_' . $m;

    $sql = sprintf('SELECT t1.entity_id FROM %s AS t1 INNER JOIN {stock} AS t2 ON t1.record_id = t2.stock_id
      WHERE stock_id = :stock_id LIMIT 1', $chado_bundle);
    $result = chado_query($sql, array(':stock_id' => $stock_id));

    if ($result->rowCount() > 0) {
      $entity_id = $result->fetchField();
    }
  }

  return $entity_id;
}
