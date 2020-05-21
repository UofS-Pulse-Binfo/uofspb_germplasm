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
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);
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
