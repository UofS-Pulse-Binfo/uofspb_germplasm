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
    // Add scripts and style.
    $field_theme_dir = drupal_get_path('module', 'germcollection') . '/includes/TripalFields/co_010__germplasm/theme/'; 
    // Style.
    drupal_add_css($field_theme_dir . 'style_table.css');
    // Script.
    drupal_add_js($field_theme_dir . 'script_stockcollection.js');
    
    // # FIELDSET: main fieldset.
    $form['main_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Germplasm'),
      '#id' => 'stock-collection-field-container',
    );

      // Warn user (admin), to review germplasm collection list before
      // clicking submit button as modifying list after would require elevated permission.
      // Will be done manually through update query.
      $form['main_fieldset']['reminder'] = array(
        '#markup' => '<div class="messages warning">Please review the final Germplasm List before clicking Save button
          To add or remove germplasm from a Stock Collection will require permission.</div>'
      );

      // # FIELD:  Select spieces.
      $species = germcollection_get_scientific_name();
      $form['main_fieldset']['fld_select_species'] = array(
        '#type' => 'select',
        '#options' => array(0 => '- Scientific Name -') + $species,
        '#prefix' => '<div class="stock-collection-fld">',
        '#suffix' => '</div>',

        '#ajax' => array(
          'event' => 'change',
          'wrapper' => 'stock-collection-wrapper-fldgermplasm', 
          'callback' => [$this, 'stock_collection_ajax_callback'],
          'progress'  => array('type' => '', 'message' => ''),
        ),

        '#id' => 'stock-collection-fld-select-species'
      );

      // # FIELD: Auto-complete search germplasm.
      // Disable everything when there is no species.
      $organism = 0;
      
      // Update organism to filter germplasm only to that organism value.
      if (isset($form_state['values']) && $form_state['values']['fld_select_species'] > 0) {
        $organism = $form_state['values']['fld_select_species'];
      }

      $disabled = (count($species) < 1) ? TRUE : FALSE; 
      $form['main_fieldset']['fld_textfield_germplasm'] = array(
        '#type' => 'textfield',
        '#size' => 40,
        '#disabled' => $disabled,
        '#attributes' => array('placeholder' => t('Germplasm / Stock Name')),
        '#prefix' => '<div id="stock-collection-wrapper-fldgermplasm" class="stock-collection-fld">',
        '#suffix' => '</div>',
        '#id' => 'stock-collection-fld-textfield-germplasm',
        '#autocomplete_path' => 'ajax/tripal/autocomplete-stockname/' . $organism
      );

      // # FIELD: Add germplasm to collection (manual insert).
      $form['main_fieldset']['fld_button_add'] = array(
        '#type' => 'button',
        '#value' => t('Add to Collection'),
        '#disabled' => $disabled,
        '#prefix' => '<div class="stock-collection-fld">',
        '#suffix' => '</div>',
        '#id' => 'stock-collection-fld-button-add',
      );

      // # LINK: Link to upload gerplasm in bulk.
      $link = l(t('Upload File'), '#', array('#attributes'));
      $form['main_fieldset']['link_upload'] = array(
        '#markup' => '<div id="stock-collection-bulk-upload" class="stock-collection-fld-right">Bulk Germplasm: ' . $link . '</div>
          <div style="clear: both"></div>'
      );

      // # CONTAINER: Contain controls for bulk germplasm upload.
      $form['main_fieldset']['container_bulk_upload'] = array(
        '#prefix' => '<div id="stock-collection-container-bulk-upload">', 
        '#suffix' => '</div>',
      );
      
        // # FIELD: Upload field object.
        $form['main_fieldset']['container_bulk_upload']['fld_upload'] = array(
          '#type' => 'file',
          '#title' => t('FILE UPLOAD: Upload Bulk Germplasm File is a TSV format with UNIQUENAME, NAME and SCIENTIFIC NAME column headers.'),
          '#disabled' => $disabled,
        );

        // # LINK: Link to upload gerplasm in bulk.
        unset($Link);
        $link = l(t('Cancel File Upload'), '#', array('#attributes'));
        $form['main_fieldset']['container_bulk_upload']['link_cancel'] = array(
          '#markup' => '<div>' . $link . '</div>'
        );

      
      
      // Container, list of germplasm active in the collection.
      $header = array();

      for($i = 0; $i < 10; $i++) {
        $l = l('Germplasm - ' . $i, '#', array('#attributes'));
        $table_rows[] = array($l . ' (Lens culinaris)', '&#x2715;');
      }

      $stock_collection = theme('table', array('sticky' => FALSE, 'header' => $header, 'rows' => $table_rows));  

      // Use id to manage items in the list.
      $id = 'stock-collection-germplasm-container';
      $form['main_fieldset']['container'] = array(
        '#markup' => '<div id="stock-collection-header">&#9205; Germplasm Collection: <span>11 Germplasm</span></div>
          <div  id="' . $id . '">' . $stock_collection . '</div>',
      );  
  }

  /**
   * Ajax callback method.  
   */  
  public function stock_collection_ajax_callback($form, $form_state) {
    return $form['main_fieldset']['fld_textfield_germplasm'];
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




