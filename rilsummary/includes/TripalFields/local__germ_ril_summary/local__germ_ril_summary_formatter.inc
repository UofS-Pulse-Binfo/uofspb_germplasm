<?php
/**
 * @class
 * Purpose: Provide a quick search on entity pages which submits/redirects to a full search.
 *
 * Display: A simple textfield search form.
 * Configuration:
 *   - path to the full search.
 *   - the URL token (query parameter) the value applies to.
 *   - help text.
 *   - textfield placeholder.
 *   - search button text.
 *   - autocomplete path.
 */
class local__germ_ril_summary_formatter extends TripalFieldFormatter {
  // The default label for this field.
  public static $default_label = 'Germplasm RIL Summary';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__germ_ril_summary');

  /**
   *  Provides the display for a field
   *
   * This function corresponds to the hook_field_formatter_view()
   * function of the Drupal Field API.
   *
   *  This function provides the display for a field when it is viewed on
   *  the web page.  The content returned by the formatter should only include
   *  what is present in the $items[$delta]['values] array. This way, the
   *  contents that are displayed on the page, via webservices and downloaded
   *  into a CSV file will always be identical.  The view need not show all
   *  of the data in the 'values' array.
   *
   *  @param $element
   *  @param $entity_type
   *  @param $entity
   *  @param $langcode
   *  @param $items
   *  @param $display
   *
   *  @return
   *    An element array compatible with that returned by the
   *    hook_field_formatter_view() function.
   */
  public function view(&$element, $entity_type, $entity, $langcode, $items, $display) {
    $module_path = drupal_get_path('module', 'rilsummary');
    $path = array(
      'base' => $GLOBALS['base_url'],
      'module' => $module_path,
      'field'   => $module_path . '/includes/TripalFields/local__germ_ril_summary/'
    );

    // Style information.
    drupal_add_css($path['field'] . 'theme/style_grs_field.css');
    // Data to render:
    $data = $items[0]['value'];

    // Table header Fs.
    // Data row.
    $f = ($data['generations']) ? explode('#', $data['generations']) : array();
    $generations_value = array();
    foreach($f as $g) {
      if (!empty($g)) {
        @list($f_gen, $f_val) = explode('=>', $g);
        $generations_value[ trim($f_gen) ] = $f_val;
      }
    }

    for($i = 1; $i < 9; $i++) {
      $tbl_args['header'][] = '<img src="' . $path['base'] . '/' . $path['field'] . 'theme/header-f' . $i . '.jpg' . '" alt="' . 'F' . $i . '" title="' . 'F' . $i . '" />';
      $tbl_args['rows'][0][] = (isset($generations_value['F' . $i])) ? $generations_value['F' . $i] : '&nbsp;';
    }


    // RIL Finished header. RIL complete.
    $tbl_args['header'][] = '<img src="' . $path['base'] . '/' . $path['field'] . 'theme/header-finished.jpg' . '" alt="RIL Complete" title="RIL complete" />
      <small>RIL Complete</small>';

    if (isset($data['complete'])) {
      if ($data['complete'] == '') {
        $attr_title = 'RIL Complete';
        // Property exists. It is complete - a check.
        $cell_val = '<img src="' . $path['base'] . '/' . $path['field'] . 'theme/message-16-ok.png" alt="' . $attr_title . '" title="' . $attr_title . '" />';
      }
      else {
        // A value in here, show the value.
        $cell_val = $data['complete'];
      }
    }
    else {
      // Nothing there.
      $cell_val = '&nbsp';
    }

    $tbl_args['rows'][0][] = $cell_val;

    // Table generations.
    $tbl_args['attributes'] = array('id' => 'field-germ-ril-summary-table');
    $table = theme('table', $tbl_args);

    // Format parents:
    $mom = (empty(trim($data['maternal']))) ? 'Unknown' : l($data['maternal'], $path['base'] . '/bio_data/' . $data['mom_entity_id']);
    $dad = (empty(trim($data['paternal']))) ? 'Unknown' : l($data['paternal'], $path['base'] . '/bio_data/' . $data['dad_entity_id']);

    // All set, render items.
    $element[0] = array(
      '#type' => 'markup',
      '#markup' => '<div id="field-germ-ril-summary-wrapper">' . $table  .

      '<small>Values indicate number of sublines in a generation.</small>
       <ul>
         <li>' . $data['name'] . ' (' . $data['uniquename'] . ')' . ' is '. ucfirst($data['type']) .'</li>
         <li>Maternal and Paternal Parents: ' . $mom . '/' . $dad . '</li>
       </ul>

      </div>',
    );


    return $element;
  }
}
