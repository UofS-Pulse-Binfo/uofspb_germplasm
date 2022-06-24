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

    // Table components.
    $summary_tables = '';    
    $title = "<div><span>%s</span>: <span>%d Germplasm</span> <span>%s</span></div>";
    $header = array('Name', 'Accession', 'Source Species', 'Origin');
    $query_string = 'experiment=%d&collection=%d';
    
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
        
        $download_link = '';
        $collection_table = '';

        if(user_is_logged_in() && user_access('access germcollection')) {
          $query_string = sprintf($query_string, $item['chado-project__project_id'], $item['chado-project_stockcollection__stockcollection_id']);
          $download_link = l('Download Collection', 'download-germplasm-collection/' . base64_encode($query_string),
            array('attributes' => array('target' => '_blank'))
          );

          $collection_table = theme('table', array('sticky' => FALSE, 'header' => $header, 'rows' => $table_rows));  
          $collection_table = '<div>' . $collection_table . '</div>';
        }

        $summary_tables = sprintf($title, $link, count($rows), $download_link);
        $summary_tables .= $collection_table;
      }
    }

    // Refer to this id for styling field elements.
    $id = 'germplasm-collection-field-container';
    $field_markup = '<div id="' . $id . '">' . $summary_tables . '</div>';
    $element[0] = [
      '#type' => 'markup',
      '#markup' => $field_markup,
      '#attached' => array(
        'css' => array(drupal_get_path('module', 'germcollection') . '/includes/TripalFields/local__project_germcollection/theme/style_table.css')
      )
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
