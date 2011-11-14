/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*-
 *
 * Copyright (C) 2011 Colin Walters <walters@verbum.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Colin Walters <walters@verbum.org>
 */

#include "config.h"

#include <gio/gio.h>

#include <string.h>

#include "ot-builtins.h"

static OstreeBuiltin builtins[] = {
  { "checkout", ostree_builtin_checkout, 0 },
  { "diff", ostree_builtin_diff, 0 },
  { "init", ostree_builtin_init, 0 },
  { "commit", ostree_builtin_commit, 0 },
  { "compose", ostree_builtin_compose, 0 },
  { "log", ostree_builtin_log, 0 },
#ifdef HAVE_LIBSOUP_GNOME
  { "pull", ostree_builtin_pull, 0 },
#endif
  { "fsck", ostree_builtin_fsck, 0 },
  { "remote", ostree_builtin_remote, 0 },
  { "rev-parse", ostree_builtin_rev_parse, 0 },
  { "remote", ostree_builtin_remote, 0 },
  { "run-triggers", ostree_builtin_run_triggers, 0 },
  { "show", ostree_builtin_show, 0 },
  { NULL }
};

static int
usage (char **argv, gboolean is_error)
{
  OstreeBuiltin *builtin = builtins;
  void (*print_func) (const gchar *format, ...);

  if (is_error)
    print_func = g_printerr;
  else
    print_func = g_print;

  print_func ("usage: %s --repo=PATH COMMAND [options]\n",
              argv[0]);
  print_func ("Builtin commands:\n");

  while (builtin->name)
    {
      print_func ("  %s\n", builtin->name);
      builtin++;
    }
  return (is_error ? 1 : 0);
}


int
main (int    argc,
      char **argv)
{
  OstreeBuiltin *builtin;
  const char *cmd;
  const char *repo;

  g_type_init ();

  g_set_prgname (argv[0]);

  builtin = builtins;

  if (argc < 3)
    return usage (argv, 1);
  
  if (!g_str_has_prefix (argv[1], "--repo="))
    return usage (argv, 1);
  repo = argv[1] + strlen ("--repo=");

  cmd = argv[2];

  while (builtin->name)
    {
      GError *error = NULL;
      if (strcmp (cmd, builtin->name) == 0)
        {
          int i;
          int tmp_argc;
          char **tmp_argv;

          tmp_argc = argc - 2;
          tmp_argv = g_new0 (char *, tmp_argc + 1);

          tmp_argv[0] = (char*)builtin->name;
          for (i = 0; i < tmp_argc; i++)
            tmp_argv[i+1] = argv[i+3];
          if (!builtin->fn (tmp_argc, tmp_argv, repo, &error))
            {
              g_free (tmp_argv);
              g_printerr ("%s\n", error->message);
              g_clear_error (&error);
              return 1;
            }
          g_free (tmp_argv);
          return 0;
        }
      builtin++;
    }
  
  g_printerr ("Unknown command '%s'\n", cmd);
  return usage (argv, 1);
}
