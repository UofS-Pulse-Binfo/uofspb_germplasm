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
      // Collection id number.
      $collection_id = $entity->chado_record_id;
      // Count germplasm in collection.
      $collection_count = count($items);

      // Selections made to # of germplasm per page.
      $params = (isset($_POST['germcollection-item-per-page'])) 
        ? (int) $_POST['germcollection-item-per-page'] : 0;
      
      if ($params && $params >= 1 && $params <= 3) {
        // Cache # of germplasm value selected to make it available
        // thoughout every page in the pager.
        $_SESSION['germcollection']['items_per_page'] = $params;
        $per_page = $params;
      }
      
      // Default # of germplasm to 1 : all germplasm but
      // check cache and see it was previously set and use it instead.
      if ($params === 0) {
        if ($_SESSION['germcollection']['items_per_page']) {
          $per_page = $_SESSION['germcollection']['items_per_page'];
        }
      }

      // Translate # of germplasm value to a number where
      // 1 - show all, 2 - 100/per page, 3 - 50/per page and 0 : not set.
      switch($per_page) { 
        case 2:
          // 100 per page.
          $germplasm_per_page = 100;
          $n = '100';
          break;

        case 3:
          // 50 per page.
          $germplasm_per_page = 50;
          $n = '50';
          break;

        default:
          // Default, all of it in one page.
          $germplasm_per_page = $collection_count;
          $n = t('All');
      }
     
      # FIELD : select number of germplasm shown per page.    
      $fld_items_per_page = [
        '#type' => 'select',
        '#title' => t('# per page'),
        '#title_display' => 'invisible',
        '#options' => [
          0 => t('# per page : @n', ['@n' => $n]), 
          1 => t('All'), 
          2 => 100, 
          3 => 50
        ],
        '#attributes' => [
          'onchange' => 'if (this.value > 0) this.form.submit();',
          'name' => 'germcollection-item-per-page'
        ],
      ];
      
      // Outbound link to search germplasm with collection search
      // criterial pre-selected.
      $link =  $GLOBALS['base_url'] . '/search/germplasm/all?collection=' . $collection_id;
      $filter_link = l(t('Filter Collection'), $link, ['attributes' => ['target' => '_blank']]);
      
      // Construct germplasm summary table.
      $collection_table = [
        '#theme' => 'table',
        '#header' => ['#', t('Name'), t('Accession'), t('Type'), t('Species')],
        '#rows' => [],
        '#empty' => t('No germplasm in this collection.')
      ];

      // Now assuming there is content, we want to add a pager.
      $items_per_page = $germplasm_per_page;
      $total_records = $collection_count;
      $total_pages = (int) ($total_records / $items_per_page) + 1;
      $pelement = 0;
      $current_page = pager_default_initialize($total_records, $items_per_page, $pelement);

      // Now only show the elements for the current page.
      $start_item = $current_page * $items_per_page;
      $end_item = $start_item + ($items_per_page - 1);
      for ($key = $start_item; $key <= $end_item; $key++) {
        if (isset($items[$key])) {
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
            ($key + 1),
            $name,
            $accession,
            $i['value']['rdfs:type'],
            $organism,
          ];

          if (isset($i['value']['internal:country_of_origin'])) {
            $row[] = $i['value']['internal:country_of_origin'];
            $collection_table['#header'][5] = 'Country of Origin';
          }

          $collection_table['#rows'][] = $row;
        }
      }

      $pager = theme('pager', [
        'tags' => [],
        'element' => $pelement,
        'parameters' => [],
        'quantity' => $total_pages,
      ]);

      $pager = $this->ajaxifyPager($pager, $entity);

      // Prepare markup of page controls (# of item per page and links)
      // and germplasm summary table.
      $field_markup = theme('germcollection_collection_view', array(
        'fld_no_per_page' => $fld_items_per_page,
        'context_links' => $filter_link,
        'germplasm_count' => $collection_count,
        'collection_table' => $collection_table,
      ));

      $field_theme_dir = drupal_get_path('module', 'germcollection') . '/theme/'; 
      $element[0] = [
        '#type' => 'markup',
        '#markup' => $field_markup . $pager,
        '#attached' => [
          'css' => [$field_theme_dir . 'style/style-collection-view.css'],
        ]
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
