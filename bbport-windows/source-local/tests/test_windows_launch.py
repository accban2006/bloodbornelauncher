from paths import ROOT
import importlib.util
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

spec = importlib.util.spec_from_file_location('launcher_win', ROOT / 'launcher/bbport_launcher_win.py')
launcher = importlib.util.module_from_spec(spec)
spec.loader.exec_module(launcher)


class DetachedLaunchTests(unittest.TestCase):
    def test_closing_launcher_uses_a_file_instead_of_an_orphaned_pipe(self):
        with tempfile.TemporaryDirectory(dir=ROOT / 'out') as directory:
            user = Path(directory) / '保存한국어'
            settings = {'user_dir': str(user), 'close_on_play': True}
            command = [sys.executable, '-c', "import time; time.sleep(.05); print('log survives launcher close')"]
            with patch.object(launcher, 'run_command', return_value=command), \
                    patch.object(launcher, 'game_environment', return_value=None):
                process = launcher.start_game_process(settings)
            try:
                self.assertIsNone(process.stdout)
                self.assertEqual(process.wait(timeout=10), 0)
                self.assertIn('log survives launcher close',
                              (user / 'last_run.log').read_text(encoding='utf-8'))
            finally:
                if process.poll() is None:
                    process.kill()
                    process.wait()

    def test_open_launcher_keeps_live_output(self):
        with patch.object(launcher, 'run_command', return_value=[sys.executable, '-c', "print('live log')"]), \
                patch.object(launcher, 'game_environment', return_value=None):
            process = launcher.start_game_process({'close_on_play': False})
        output, _ = process.communicate(timeout=10)
        self.assertEqual(process.returncode, 0)
        self.assertIn(b'live log', output)


if __name__ == '__main__':
    unittest.main()
