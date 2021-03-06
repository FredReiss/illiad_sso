<?php

// contents
//  contents
//  custom_form
//  custom_form_sooner_id
//  custom_submit_form_sooner_id


// a helper for forms nestled in the above custom false-"mvc" format
function illiad_sso_custom_form($name, $values=NULL, $omit_prefix=FALSE){
 $module_name = "illiad_sso";
 $mvc = illiad_sso_custom_mvc($name);
 $form_data = $mvc["form"];
 $form = array();
 foreach($form_data["fields"] as $field_data){
  $prefix = $omit_prefix ? "" : $module_name . "_";
  $field_name = $prefix . $field_data["field_name"];
  $field = $field_data["field"];
  if(isset($values) && array_key_exists($field_data["field_name"], $values))
   $field["value"] = $values[$field_data["field_name"]];
  else{
   $fallback_value = $field_data["default"];
   $default_variable = $field_name;
   $default_value = variable_get($default_variable, $fallback_value);
   $field["#default_value"] = $default_value;
  }
  $form[$field_name] = $field;
 }
 return $form;
}

// form builder for settings page
function illiad_sso_custom_form_settings(){
 return system_settings_form(illiad_sso_custom_form("settings"));
}


// form builder for missing Sooner ID on redirect page
function illiad_sso_custom_form_sooner_id(){
 $phone_link = "";
 $phone_link = " or <a href=\"tel:+14053253113\">(405)-325-3113</a>";
 $phone_link = " or (405)-325-3113";
 $sooner_card_email = "<a href=\"mailto:soonercard@ou.edu\">soonercard@ou.edu</a>";
 $sooner_card_email = "soonercard@ou.edu";
 $contact_message = t("For help finding this information, please contact the Sooner Card Office at " . $sooner_card_email . $phone_link . ".");
 $contact_message = "";
 $sooner_id_field = array(
  "#type" => "textfield",
  "#title" => t("Sooner ID"),
  "#description" => $contact_message,
  "#required" => TRUE,
  "#element_validate" => array("element_validate_integer_positive"), // TODO: custom validator
  "#maxlength" => 9,
 );
 $confirm_sooner_id_field = array(
  "#title" => t("Confirm Sooner ID"),
  "#description" => t("Type your Sooner ID again."),
 );
 $confirm_sooner_id_field += $sooner_id_field;
 return array(
  "sooner_id" => $sooner_id_field,
  "confirm_sooner_id" => $confirm_sooner_id_field,
  "buttons" => array(
   "submit" => array(
    "#type" => "submit",
    "#value" => t("Set your Sooner ID")
   )
  ),
  "#submit" => array("illiad_sso_custom_submit_form_sooner_id")
 );
}

function illiad_sso_custom_submit_form_sooner_id($form, &$form_state){
 global $user;
 $product_name = "ILLiad";

 $redirect_url = $form_state["values"]["illiad_sso_redirect_url"];
 $sooner_id = $form_state["values"]["sooner_id"];
 $confirm_sooner_id = $form_state["values"]["confirm_sooner_id"];
 $error = "";
 if($sooner_id != $confirm_sooner_id)
  $error = t("Sooner ID must match.");
 if(9 != strlen($sooner_id))
  $error = t("Sooner ID must be exactly 9 digits.");
 if($error){
  drupal_set_message($error, "error");
  header("Location: " . $redirect_url);
  exit();
 }

 $account = user_load($user->uid);
 $sooner_id_field_name = _illiad_sso_get_settings_variable("sooner_id profile_field name");

 $sooner_id_field = field_get_items("user", $account, $sooner_id_field_name);
 if($sooner_id_field)
  if(array_key_exists(0, $sooner_id_field))
   if(array_key_exists("value", $sooner_id_field[0]))
    if($sooner_id_field[0]["value"]){
     drupal_set_message("Your Sooner ID was already set.", "warning");
     return;
    }

 $edit = array();
 $edit[$sooner_id_field_name] = array("und" => array(array("value" => $sooner_id)));
 user_save($account, $edit);

 header("Location: " . $redirect_url);
 exit();
}
