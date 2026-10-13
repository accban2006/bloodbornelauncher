import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
sys.path.insert(0, str(ROOT / 'scripts'))
import mods
import patches
import run


class WindowsTextTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(dir=ROOT / 'tests')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def japanese_locale(self):
        original = Path.open

        def open_in_locale(path, mode='r', buffering=-1, encoding=None,
                           errors=None, newline=None):
            if 'b' not in mode and encoding is None:
                encoding = 'cp932'
            return original(path, mode, buffering, encoding, errors, newline)

        return patch.object(Path, 'open', open_in_locale)

    def test_ini_utf8_on_japanese_windows(self):
        config = self.root / 'bbport.ini'
        config.write_text('# 中文 한국어 日本語\nupscaler=off\nplayer_name=한국어\n', encoding='utf-8')
        with self.japanese_locale():
            self.assertEqual(patches.read_settings(config),
                             {'upscaler': 'off', 'player_name': '한국어'})

    def test_mod_names_utf8_on_japanese_windows(self):
        name = '中文한국어'
        (self.root / name / 'chr').mkdir(parents=True)
        config = self.root / 'mods.json'
        config.write_text(json.dumps({'disabled': [name]}, ensure_ascii=False), encoding='utf-8')
        with self.japanese_locale():
            self.assertEqual(mods.selected(self.root, config), [])

    def test_external_patch_names_utf8_on_japanese_windows(self):
        name = '中文한국어.xml/修正'
        config = self.root / 'patches.json'
        config.write_text(json.dumps({'enabled': [name]}, ensure_ascii=False), encoding='utf-8')
        metadata = patches.ET.Element('Metadata', isEnabled='false')
        found = [(name, self.root / 'patch.xml', metadata)]
        with self.japanese_locale():
            self.assertEqual(patches.external_selection(found, config), found)

    def test_capture_unicode_path_with_legacy_console_encoding(self):
        scripts = self.root / 'scripts'
        scripts.mkdir()
        (scripts / 'echo.py').write_text("print('C:/ゲーム/中文/한국어')\n", encoding='utf-8')
        with patch.object(run, 'PORT', self.root), patch.object(run, 'no_console', return_value=0), \
                patch.dict('os.environ', {'PYTHONIOENCODING': 'cp1252'}):
            self.assertEqual(run.run_script('echo.py', capture=True).strip(),
                             'C:/ゲーム/中文/한국어')


if __name__ == '__main__':
    unittest.main()
