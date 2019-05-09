<?php
/**
 * @class
 * Purpose:
 *
 * Allowing edit?
 * Data:
 * Assumptions:
 */
class local__project_germcollection_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Project-Related Germplasm Collection';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('local__project_germcollection');

  /**
   * @see ChadoFieldWidget::form()
   *
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);
    $field_name = $this->field['field_name'];
    $pkey = 'project_stockcollection_id';

    // Get the field defaults.
    $project_id = NULL;
    $stockcollection_id = NULL;
    $stockcollection_name = NULL;
    $pkey_value = NULL;

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (count($items) > 0) {
      $project_id = tripal_get_field_item_keyval($items, $delta, 'chado-project_stockcollection__project_id', $project_id);
      $stockcollection_id = tripal_get_field_item_keyval($items, $delta, 'chado-project_stockcollection__stockcollection_id', $stockcollection_id);
      $pkey_value = tripal_get_field_item_keyval($items, $delta, 'chado-project_stockcollection__'.$pkey, $pkey_value);
      $stockcollection_name = $items[$delta]['value']['schema:name'];
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $project_id = $form_state['values'][$field_name]['und'][$delta]['chado-project_stockcollection__project_id'];
      $stockcollection_id = $form_state['values'][$field_name]['und'][$delta]['chado-project_stockcollection__stockcollection_id'];
      $pkey_value = $form_state['values'][$field_name]['und'][$delta]['chado-project_stockcollection__'.$pkey];
      $stockcollection_name = $form_state['values'][$field_name]['und'][$delta]['value']['schema:name'];
    }

    $widget['chado-project_stockcollection__project_id'] = array(
      '#type' => 'hidden',
      '#value' => $project_id,
    );

    $widget['chado-project_stockcollection__stockcollection_id'] = array(
      '#type' => 'hidden',
      '#value' => $stockcollection_id,
    );

    $widget['chado-project_stockcollection__'.$pkey] = array(
      '#type' => 'hidden',
      '#value' => $pkey_value,
    );

    $widget['stockcollection_name'] = array(
      '#type' => 'textfield',
      '#title' => 'Germplasm Collection',
      '#description' => 'Enter the name of the germplasm collection used for this experiment. The collection must already exist.',
      '#default_value' => $stockcollection_name,
    );
  }

  /**
   * @see ChadoFieldWidget::validate()
   *
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];

    $value = $form_state['values'][$field_name]['und'][$delta]['chado-stockcollection__name'];
    $form_state['values'][$field_name]['und'][$delta]['value'] = $value;

    $project_id = $form_state['values'][$field_name]['und'][$delta]['chado-project_stockcollection__project_id'];
    $stockcollection_id = $form_state['values'][$field_name]['und'][$delta]['chado-project_stockcollection__stockcollection_id'];
    $stockcollection_name = $form_state['values'][$field_name]['und'][$delta]['stockcollection_name'];

    if (!$project_id AND isset($element['#entity']->chado_record_id)) {
      $project_id = $element['#entity']->chado_record_id;
    }

    if (!$stockcollection_id AND $stockcollection_name) {
      $stockcollection_id = chado_query('SELECT stockcollection_id FROM {stockcollection} WHERE name=:name',
	array(':name' => trim($stockcollection_name)))->fetchField();
    }

    if ($stockcollection_id) {
      $form_state['values'][$field_name]['und'][$delta]['chado-project_stockcollection--project_id'] = $project_id;
      $form_state['values'][$field_name]['und'][$delta]['chado-project_stockcollection__stockcollection_id'] = $stockcollection_id;
    }
    elseif ($stockcollection_name) {
      form_set_error(
        $this->field['field_name'] . '][' . $langcode . '][' . $delta . '][stockcollection_name',
        'Collection does not exists.'
      );
    }
  }

  /**
   * @see ChadoFieldWidget::submit()
   *
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {

  }

}
