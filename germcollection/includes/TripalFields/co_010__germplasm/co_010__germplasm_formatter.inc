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
      
      // Search a specific germplasm in a collection.
      $search = (isset($_POST['germcollection-search'])) 
        ? strip_tags($_POST['germcollection-search']) : '';
        
      // Extract id of germplasm to search since the search box
      // formats the autocomplete selection to stock name [id: stock id]
      $search_stock_id = 0;
      $found_stock_id = '';
      $search_result = '';
      $found_flag = 0;

      if ($search != '') {
        preg_match('/\[id:(\d+)\]$/', $search, $match);
        if (isset($match[1]) && $match[1] > 0) {
          $search_stock_id = $match[1];

          $stock_ids = array_column($items, 'chado-stockcollection__stock_id');
          $found_stock_id = array_search($search_stock_id, $stock_ids);
          if ($found_stock_id === FALSE) {
            $search_result = 'Not found';
          }
          else {
            $found_flag = 1;
          }
        }
        else {
          $search_result = 'Not found';
        }
      }  

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

      # FIELD: search box text field.
      // Search is available when items shown per page
      // is all items.
      $fld_search = [
        '#type' => 'textfield',
        '#id' => 'germcollection-fld-textfield-search',
        '#attributes' => [
          'placeholder' => 'Stock Name', 
          'name' => 'germcollection-search'
        ],
        '#disabled' => TRUE
      ];
      
      if ($per_page > 1) {
        $fld_search['#attributes']['disabled'] = 'disabled';
      }
      
      // Download link - export list to a csv file.
      // Identical to project collection, this link is available only with
      // access germcollection permission set.
      if(user_is_logged_in() && user_access('access germcollection')) {
        $download_link = l(t('Download'), 'download-germplasm-collection/' . base64_encode('collection=' . $collection_id),
          ['attributes' => ['target' => '_blank']]
        );
      }

      // Outbound link to search germplasm with collection search
      // criterial pre-selected.
      $link =  $GLOBALS['base_url'] . '/search/germplasm/all?collection=' . $collection_id;
      $filter_link = l(t('Filter'), $link, ['attributes' => ['target' => '_blank']]);
      
      // Germplasm count as link to reload all germplasm in a collection
      // in one clickable link.
      $link =  $GLOBALS['base_url'] . '/germplasm-collection/' . $entity->chado_record->uniquename;
      $count_link = l(t($collection_count . ' Germplasm'), $link);

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
      if ($found_flag == 1) {
        $start_item = $found_stock_id;
        $end_item = $found_stock_id;
        $items_per_page = 1;
        $total_pages = 0;
      }

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
            ($found_stock_id != '') ? 1 : ($key + 1),
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
        'fld_search' => $fld_search,
        'txt_search_result' => $search_result,
        'context_links' => $download_link . ' | ' . $filter_link,
        'germplasm_count' => $count_link,
        'collection_table' => $collection_table,
      ));

      $field_theme_dir = drupal_get_path('module', 'germcollection') . '/theme/'; 
      $host = url($GLOBALS['base_url'], ['absolute' => true]);
      // Settings:
      drupal_add_js(['germcollection' => [
        'autocomplete' => '/ajax/tripal/autocomplete-germcollection/' . $collection_id . '/',
        'host' => $host . '/' 
      ]], ['type' => 'setting']);

      $element[0] = [
        '#type' => 'markup',
        '#markup' => $field_markup . $pager,
        '#attached' => [
          'css' => [$field_theme_dir . 'style/style-collection-view.css'],
          'js' => [$field_theme_dir . 'script/script-collection-view.js'],
          'library' => [['system', 'ui.autocomplete']],
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
