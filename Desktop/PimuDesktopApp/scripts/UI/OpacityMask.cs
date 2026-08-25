using Godot;
using System;
using System.Collections.Generic;

namespace J113D.Pimu.Desktop.App.UI
{
    public class OpacityMask
    {
        public const float TransparencyThreshold = 0.1f;

        private static readonly Dictionary<string, OpacityMask> _opacityMasks = [];

        private readonly byte[] _data;
        private readonly int _rowLength;

        public int Width { get; }
        public int Height { get; }

        public Vector2I Resolution { get; set; }


        private OpacityMask(int width, int height)
        {
            Width = width;
            Height = height;
            Resolution = new(width, height);

            _rowLength = width / 8;
            if(width % 8 > 0)
            {
                _rowLength++;
            }

            _data = new byte[_rowLength * height];
        }

        private void ReadData(Texture2D source)
        {
            Image image = source.GetImage();

            for(int y = 0; y < Height; y++)
            {
                for(int bx = 0; bx < _rowLength; bx++)
                {
                    byte block = 0;
                    int bitCount = int.Min(8, Width - (bx * 8));
                    for(int x = 0; x < bitCount; x++)
                    {
                        if(image.GetPixel((bx * 8) + x, y).A > TransparencyThreshold)
                        {
                            block |= (byte)(1 << x);
                        }
                    }

                    _data[bx + (y * _rowLength)] = block;
                }
            }
        }

        public static bool CheckInsideOpaque(Vector2 point, TextureRect control)
        {
			if (!new Rect2(Vector2.Zero, control.Size).HasPoint(point))
            {
                return false;
            }

			return control.Texture == null || GetOpacityMask(control.Texture).CheckOpaque(point, control);
        }

        public bool CheckOpaque(Vector2 point, Control control)
        {
            int pixelX = int.Clamp((int)Math.Round(point.X / control.Size.X * Width), 0, Width - 1);
            int pixelY = int.Clamp((int)Math.Round(point.Y / control.Size.Y * Height), 0, Height - 1);

            int bPixelX = pixelX / 8;
            int bPixelMask = 1 << (pixelX % 8);

            int pixelIndex = bPixelX + (pixelY * _rowLength);
            byte pixel = _data[pixelIndex];

            return (pixel & bPixelMask) != 0;
        }

        public static OpacityMask GetOpacityMask(Texture2D texture)
        {
            if(!_opacityMasks.TryGetValue(texture.ResourcePath, out OpacityMask? result))
            {
                result = new(texture.GetWidth(), texture.GetHeight());

                try
                {
					result.ReadData(texture);
                }
                catch(Exception exception)
                {
                    GD.PushWarning($"Can't generate opacity mask for \"{texture.ResourcePath}\":\n" + exception.Message);
                    throw;
                }

                _opacityMasks.Add(texture.ResourcePath, result);
            }

            return result;
        }

    }
}
