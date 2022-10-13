<?php
/**
 * @class
 * Purpose: Supply a germplasm list for a stockcollection.
 *
 * Allowing edit? No
 * Data:
 *   Details for each germplasm are stored in stock and stockprop. The
 *   connection between stockcollection and stocks is in stockcollection_stock.
 * Assumptions: None known.
 */
class co_010__germplasm_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Germplasm List';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['co_010__germplasm'];


  /**
   * @see ChadoFieldWidget::form()
   *
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    // # FIELD: main fieldset.
    $widget['main_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Germplasm'),
    );
      
      // Warn user (admin), to review germplasm collection list before
      // clicking submit button as modifying list after would require elevated permission.
      // Will be done manually through update query.
      $widget['main_fieldset']['reminder'] = array(
        '#markup' => '<div class="messages warning">Please review the final Germplasm List before clicking Save button
          To add or remove germplasm from a Stock Collection will require permission.</div>'
      );

      // # FIELD:  Select spieces.
      $widget['main_fieldset']['fld_select_species'] = array(
        '#type' => 'select'
      );

      // # FIELD: Auto-complete search germplasm.
      $widget['main_fieldset']['fld_textfield_germplasm'] = array(
        '#type' => 'textfield'
      );

      // # FIELD: Add germplasm to collection (manual insert).
      $widget['main_fieldset']['fld_button_add'] = array(
        '#type' => 'button',
        '#value' => t('Add to Collection')
      );

      // # LINK: Link to upload gerplasm in bulk.
      $link = l(t('Upload File'), '#', array('#attributes'));
      $widget['main_fieldset']['link_upload'] = array(
        '#markup' => '<div>Bulk Germplasm: ' . $link . '</div>'
      );

      // # FIELD: Secondary fieldset - controls for bulk germplasm upload.
      $widget['main_fieldset']['fieldset_1'] = array(
        '#type' => 'fieldset',
        '#title' => t('Upload Bulk Germplasm File is a TSV format with UNIQUENAME, NAME and SCIENTIFIC NAME column headers.')
      );
      
        // # FIELD: Upload field object.
        $widget['main_fieldset']['fieldset_1']['fld_upload'] = array(
          '#type' => 'file',
        );

        // # LINK: Link to upload gerplasm in bulk.
        unset($Link);
        $link = l(t('Cancel File Upload'), '#', array('#attributes'));
        $widget['main_fieldset']['fieldset_1']['link_cancel'] = array(
          '#markup' => '<div>' . $link . '</div>'
        );
      
      // Container, list of germplasm active in the collection.
      $header = array();

     for($i = 0; $i < 10; $i++) {
        $l = l('Germplasm - ' . $i, '#', array('#attributes'));
        $table_rows[] = array($l . ' (Lens culinaris)', '&#x2715;');
     }

      $stock_collection = theme('table', array('sticky' => FALSE, 'header' => $header, 'rows' => $table_rows));  


      $id = 'stock-collection-field-container';
      $widget['main_fieldset']['container'] = array(
        '#markup' => '<div id="' . $id . '">&#9205; Germplasm Collection: <span>11 Germplasm</span></div>
          <div>' . $stock_collection . '</div>',
        '#attached' => array(
          'css' => array(drupal_get_path('module', 'germcollection') . '/includes/TripalFields/co_010__germplasm/theme/style_table.css')
        )  
      );  
  }

  /**
   * @see ChadoFieldWidget::validate()
   *
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
  }

  /**
   * @see ChadoFieldWidget::submit()
   *
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
  }

}
