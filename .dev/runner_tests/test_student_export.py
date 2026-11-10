"""Check that student exports include only deliverables and preserve executables."""
import os
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from export_student import export_student


class StudentExportTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory(prefix='hw5 export ')
        self.addCleanup(temporary.cleanup)
        self.workspace = Path(temporary.name)
        self.manifest = self.workspace / '.dev/student-files.txt'
        self.write('.dev/student-files.txt',
                   'Makefile\nsrc/search.cpp\n.guides/secure/grade.py\n'
                   '.guides/tests/references/instructor/bin/shipwreck_reference\n')
        self.write('Makefile', 'all:\n')
        self.write('src/search.cpp', '// starter code\n')
        self.write('.guides/secure/grade.py', '# protected grader\n')
        self.binary = self.write(
            '.guides/tests/references/instructor/bin/shipwreck_reference', '#!/bin/sh\n')
        self.binary.chmod(0o755)

    def write(self, name, content):
        path = self.workspace / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content)
        return path

    def test_exports_only_listed_files_including_hidden_grader_and_executable(self):
        for name in ('.dev/references/instructor/src/search.cpp', '.git/config',
                     'src/notes.txt', '.guides/tests/check.log',
                     '.guides/tests/runner/__pycache__/execution.pyc'):
            self.write(name, 'private or generated\n')
        destination = export_student(self.workspace)
        actual = {str(path.relative_to(destination))
                  for path in destination.rglob('*') if path.is_file()}
        self.assertEqual(actual, set(self.manifest.read_text().splitlines()))
        self.assertTrue(os.access(str(destination / self.binary.relative_to(self.workspace)), os.X_OK))
        self.assertEqual((destination / 'src/search.cpp').read_text(), '// starter code\n')

    def test_reexport_removes_stale_files_and_updates_deliverables(self):
        destination = export_student(self.workspace)
        (destination / 'obsolete.cpp').write_text('stale')
        self.write('src/search.cpp', '// updated starter\n')
        export_student(self.workspace)
        self.assertFalse((destination / 'obsolete.cpp').exists())
        self.assertEqual((destination / 'src/search.cpp').read_text(), '// updated starter\n')

    def test_missing_deliverable_keeps_previous_export(self):
        destination = export_student(self.workspace)
        (self.workspace / 'src/search.cpp').unlink()
        with self.assertRaisesRegex(ValueError, 'Missing student file'):
            export_student(self.workspace)
        self.assertEqual((destination / 'src/search.cpp').read_text(), '// starter code\n')

    def test_manifest_cannot_include_private_paths_or_escape_workspace(self):
        for name in ('.dev/student-files.txt', '.git/config', '../secret', '/etc/passwd'):
            with self.subTest(name=name):
                self.manifest.write_text(name + '\n')
                with self.assertRaises(ValueError):
                    export_student(self.workspace)

    def test_symlinked_source_and_source_directory_are_rejected(self):
        source = self.workspace / 'src/search.cpp'
        source.unlink()
        source.symlink_to(self.manifest)
        with self.assertRaisesRegex(ValueError, 'symlinks'):
            export_student(self.workspace)
        source.unlink()
        (self.workspace / 'src').rmdir()
        (self.workspace / 'src').symlink_to(self.workspace / '.dev', target_is_directory=True)
        with self.assertRaisesRegex(ValueError, 'symlinks'):
            export_student(self.workspace)

    def test_export_cannot_follow_symlinked_destination(self):
        directory = self.workspace / 'dist'
        directory.mkdir()
        (directory / 'student').symlink_to(self.workspace / 'src', target_is_directory=True)
        with self.assertRaisesRegex(ValueError, 'symlink'):
            export_student(self.workspace)
        self.assertTrue((self.workspace / 'src/search.cpp').is_file())


if __name__ == '__main__':
    unittest.main()
