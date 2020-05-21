<?php
/**
 * @class
 * Purpose:
 *
 * Display:
 * Configuration:
 */
class local__project_germcollection_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Germplasm Collection Name';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['local__project_germcollection'];

  // The list of default settings for this formatter.
  public static $default_settings = [
    'setting1' => 'default_value',
  ];

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

    // For each germplasm collection.
    foreach ($items as $delta => $item) {

      $link = '';
      if (is_array($item['value'])) {
        $link = $item['value']['schema:name'];
        if (isset($item['value']['schema:url'])) {
          $link = l($item['value']['schema:name'], $item['value']['schema:url']);
        }
      }

      $element[$delta] = [
        '#type' => 'markup',
        '#markup' => $link,
      ];
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
