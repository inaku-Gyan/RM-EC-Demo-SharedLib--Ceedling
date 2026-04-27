#pragma once

/* ----------------------------------------------------------------------------
 *  In-repo test configuration.
 *
 *  When the library is consumed as a submodule, the *business* repo provides
 *  its own sl_config.h (with #defines tailored to its target). For in-repo
 *  testing we don't have a business repo, so we rely on per-mixin -D flags
 *  injected by ceedling. Those defines are seen before the default header
 *  is included, so the #ifndef guards in sl_config_default.h take them as-is.
 * ---------------------------------------------------------------------------- */

#include "sl_config_default.h" // IWYU pragma: export
