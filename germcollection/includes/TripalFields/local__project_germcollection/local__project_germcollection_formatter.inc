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
    // Attach styling.
    drupal_add_css(drupal_get_path('module', 'germcollection') . '/includes/TripalFields/local__project_germcollection/theme/style_table.css');

    // Get the settings
    $settings = $display['settings'];
    // Table components.
    $summary_tables = '';
    $header = array('Name', 'Accession', 'Source Species', 'Origin');

    // For each germplasm collection.
    foreach ($items as $delta => $item) {
      $link = '';
      if (is_array($item['value'])) {
        $link = $item['value']['schema:name'];
        if (isset($item['value']['schema:url'])) {
          $link = l($item['value']['schema:name'], $item['value']['schema:url']);
        }
      }

      if ($item['value']['local:project_germcollection']) {
        $rows = $item['value']['local:project_germcollection'];
        foreach($rows as $row) {
          $table_rows[] = array(
            l($row[0], $GLOBALS['base_url'] . '/bio_data/' . $row[2]),
            $row[1],
            '<i>' . ucfirst($row[3]. ' ' . $row[4]) . '</i>', 
            $row[5]
          );
        }

        $summary_tables .= theme('table', array('sticky' => TRUE, 'header' => $header, 'rows' => $table_rows));  
      }
    }

    // Refer to this id for styling summary table.
    $id = 'project-germpcollection-wrapper';
    $query_string = 'experiment=' . $item['chado-project__project_id'] . '&collection=' . $item['chado-project_stockcollection__stockcollection_id'];
    $download_link = l('Download Collection', 'download-germplasm-collection/' . base64_encode($query_string),
      array('attributes' => array('target' => '_blank'))
    );

    $has_permission = TRUE;
    if(!user_is_logged_in() && !user_access('access germcollection')) {
      $has_permission = FALSE;
      $summary_tables = '';
    }

    $field_markup = '
      <div id="germplasm-collection-field-container">
        <div>' . count($rows) . '  Germplasm | ' . $download_link . '</div>
        <div id="' . $id . '" style="height: 300px; overflow-y: scroll">' . $summary_tables . '</div>
      </div>
    ';

    $markup = (!$has_permission || $summary_tables == '') 
      ? 'Germplasm collection not available' 
      : $field_markup;

    $element[0] = [
      '#type' => 'markup',
      '#markup' => $markup
    ];
  }

    /**
    * @see ChadoFieldFormatter::settingsSummary()
    *
    **/

  public function settingsSummary($view_mode) {
    return '';
  }
}
