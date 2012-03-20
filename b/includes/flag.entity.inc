<?php

/**
 * @file
 * Provides supporting code for the entity/fields system.
 *
 * Note: We're making the <em>flaggings</em> fieldable, not the <em>flags</em>.
 * (In the same way that Drupal makes <em>nodes</em> fieldable, not <em>node
 * types</em>).
 */

/**
 * Implements hook_entity_info().
 */
function flag_entity_info() {
  $return = array(
    'flagging' => array(
      'label' => t('Flagging'),
      'controller class' => 'FlaggingController',
      'base table' => 'flag_content',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'fcid',
        'bundle' => 'flag_name',
      ),
      // The following tells Field UI how to extract the bundle name from a
      // $flag object when we're visiting ?q=admin/.../manage/%flag/fields.
      'bundle keys' => array(
        'bundle' => 'name',
      ),
      'bundles' => array(),
    ),
  );

  foreach (flag_get_flags(NULL, NULL, NULL, TRUE) as $flag) {
    $return['flagging']['bundles'][$flag->name] = array(
      'label' => $flag->title,
      'admin' => array(
        'path' => FLAG_ADMIN_PATH . '/manage/%flag',
        'real path' => FLAG_ADMIN_PATH . '/manage/' . $flag->name,
        'bundle argument' => FLAG_ADMIN_PATH_START + 1,
        'access arguments' => array('administer flags'),
      ),
    );
  }

  return $return;
}

/**
 * Controller class for flaggings.
 */
class FlaggingController extends DrupalDefaultEntityController {

  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    $query = parent::buildQuery($ids, $conditions, $revision_id);
    // Add the flag name, which determines the bundle.
    $query->innerJoin('flags', 'flags', 'base.fid = flags.fid');
    $query->addField('flags', 'name', 'flag_name');
    return $query;
  }
}

/**
 * Loads a flagging entity.
 *
 * @param $fcid
 *   The 'fcid' database serial column.
 * @param $reset
 *   Whether to reset the DrupalDefaultEntityController cache.
 *
 * @return
 *   The entity object, or FALSE if it can't be found.
 */
function flagging_load($fcid, $reset = FALSE) {
  $result = entity_load('flagging', array($fcid), array(), $reset);
  return reset($result);
}

// @todo: Implement flagging_save(). It's not required but other modules may expect it.

// @todo: Implement flagging_view(). Not extremely useful. I already have it.

// @todo: When renaming a flag: Call field_attach_rename_bundle().

// @todo: When creating a flag: Call field_attach_create_bundle().

// @todo: When deleting a flag: Call field_attach_delete_bundle().

// @tood: Discuss: Should flag deleting call flag_reset_flag()? No.

// @todo: flag_reset_flag():
// - it should delete the flaggings.
// - (it has other issues; see http://drupal.org/node/894992.)
// - (is problematic: it might not be possible to delete all data in a single page request.)

// @todo: Discuss: Note that almost all functions/identifiers dealing with
// flaggings *aren't* prefixed by "flag_". For example:
//  - The menu argument is %flagging, not %flag_flagging.
//  - The entity type is "flagging", not "flag_flagging".
// On the one hand this succinct version is readable and nice. On the other hand, it isn't
// very "correct".
