<?php
/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class co_010__germplasm_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Germplasm List';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('co_010__germplasm');

  // The list of default settings for this formatter.
  public static $default_settings = array(
    'setting1' => 'default_value',
  );

   /**
    * @see ChadoFieldFormatter::settingsForm()
    *
    **/
  public function settingsForm($view_mode, $form, &$form_state) {

  }

    /**
    * @see ChadoFieldFormatter::View()
    *
    **/
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];

    $element[0] = array(
      '#theme' => 'table',
      '#header' => array('Name', 'Accession', 'Type', 'Species', 'Country of Origin'),
      '#rows' => array(),
    );

    foreach ($items as $i) {

      // Form the link if the URL is available.
      $name = $i['value']['schema:name'];
      $accession = $i['value']['data:identifier'];
      if (isset($i['value']['schema:url'])) {
        $name = l($name, $i['value']['schema:url']);
        $accession = l($accession, $i['value']['schema:url']);
      }

      // Concatenate genus + species to for the scientific name.
      $organism = $i['value']['taxrank:0000005'] . ' ' . $i['value']['taxrank:0000006'];

      // Compile the table row.
      $element[0]['#rows'][] = array($name, $accession, $i['value']['rdfs:type'], $organism, $i['value']['internal:country_of_origin']);
    }
  }

    /**
    * @see ChadoFieldFormatter::settingsSummary()
    *
    **/
  public function settingsSummary($view_mode) {
    return '';
  }

}
