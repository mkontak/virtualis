using System;
using System.Linq.Dynamic;
using System.Linq.Expressions;
using System.Windows.Data;
using System.Windows.Markup;
using System.Collections.Generic;

namespace Viatronix.UI.Converters
{
    public class ElementExtension : MarkupExtension
    {
        public ElementExtension(string nameAndSimplePath)
        {
            this.Binding = new Binding();
            // TODO: implement more property path styles
            int pi = nameAndSimplePath.IndexOf('.');
            this.Binding.ElementName = nameAndSimplePath.Substring(0, pi);
            this.Binding.Path = new System.Windows.PropertyPath(nameAndSimplePath.Substring(pi + 1, nameAndSimplePath.Length - pi - 1));
        }

        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            return this.Binding.ProvideValue(serviceProvider);
        }

        private Binding Binding;
    }

    public class BindingExtension : MarkupExtension
    {
        public BindingExtension()
        {
            this.MultiBinding = new MultiBinding();
            this.Parameters = new List<string>();
        }

        #region Binding Parameters
        public Binding a { set { AddBinding("a", value); } }
        public Binding b { set { AddBinding("b", value); } }
        public Binding c { set { AddBinding("c", value); } }
        public Binding d { set { AddBinding("d", value); } }
        public Binding e { set { AddBinding("e", value); } }
        public Binding f { set { AddBinding("f", value); } }
        public Binding g { set { AddBinding("g", value); } }
        public Binding h { set { AddBinding("h", value); } }
        public Binding i { set { AddBinding("i", value); } }
        public Binding j { set { AddBinding("j", value); } }
        public Binding k { set { AddBinding("k", value); } }
        public Binding l { set { AddBinding("l", value); } }
        public Binding m { set { AddBinding("m", value); } }
        public Binding n { set { AddBinding("n", value); } }
        public Binding o { set { AddBinding("o", value); } }
        public Binding p { set { AddBinding("p", value); } }
        public Binding q { set { AddBinding("q", value); } }
        public Binding r { set { AddBinding("r", value); } }
        public Binding s { set { AddBinding("s", value); } }
        public Binding t { set { AddBinding("t", value); } }
        public Binding u { set { AddBinding("u", value); } }
        public Binding v { set { AddBinding("v", value); } }
        public Binding w { set { AddBinding("w", value); } }
        public Binding x { set { AddBinding("x", value); } }
        public Binding y { set { AddBinding("y", value); } }
        public Binding z { set { AddBinding("z", value); } }
        #endregion

        public string Forward
        {
            set
            {
                this.ForwardExpression = String.Concat(
                    String.Join(",", Parameters.ToArray()),
                    "=>",
                    value);
            }
        }

        public string ConvertBack
        {
            set
            {
                this.BackwardExpression = String.Concat(
                    String.Join(",", Parameters.ToArray()),
                    "=>",
                    value);
            }
        }

        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            IXamlTypeResolver tr = serviceProvider.GetService(typeof(IXamlTypeResolver)) as IXamlTypeResolver;
            IProvideValueTarget tp = serviceProvider.GetService(typeof(IProvideValueTarget)) as IProvideValueTarget;

            this.MultiBinding.Converter = new MultiConverter(this.ForwardExpression, this.BackwardExpression, tr);
            object value = this.MultiBinding.ProvideValue(serviceProvider);
            return value;
        }

        private void AddBinding(string p, Binding value)
        {
            this.Parameters.Add(p);
            this.MultiBinding.Bindings.Add(value);
        }

        private MultiBinding MultiBinding;
        private string ForwardExpression;
        private string BackwardExpression;
        private List<string> Parameters;

    }
}
