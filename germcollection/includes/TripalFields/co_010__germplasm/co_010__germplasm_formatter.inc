<?php
/**
 * @class
 * Purpose: Supply a germplasm list for a stockcollection page in table format.
 *
 * Display: Uses an HTML tabe to display germplasm.
 * Configuration: None.
 */
class co_010__germplasm_formatter extends ChadoFieldFormatter {

  // The default label for this field.
  public static $default_label = 'Germplasm List';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['co_010__germplasm'];

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

    if (sizeof($items)) {
      $element[0] = [
        '#theme' => 'table',
        '#header' => ['Name', 'Accession', 'Type', 'Species'],
        '#rows' => [],
      ];

      // Now assuming there is content, we want to add a pager.
      $items_per_page = 10;
      $total_records = count($items);
      $total_pages = (int) ($total_records / $items_per_page) + 1;
      $pelement = 0;
      $current_page = pager_default_initialize($total_records, $items_per_page, $pelement);

      // Now only show the elements for the current page.
      $start_item = $current_page * $items_per_page;
      $end_item = $start_item + ($items_per_page - 1);
      for ($key = $start_item; $key <= $end_item; $key++) {
        $i = $items[$key];

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
        $row = [
          $name,
          $accession,
          $i['value']['rdfs:type'],
          $organism,
        ];
        if (isset($i['value']['internal:country_of_origin'])) {
          $row[] = $i['value']['internal:country_of_origin'];
          $element[0]['#header'][4] = 'Country of Origin';
        }
        $element[0]['#rows'][] = $row;
      }

      $pager = theme('pager', [
        'tags' => [],
        'element' => $pelement,
        'parameters' => [],
        'quantity' => $total_pages,
      ]);
      $pager = $this->ajaxifyPager($pager, $entity);
      $element[0]['#suffix'] = $pager;
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
