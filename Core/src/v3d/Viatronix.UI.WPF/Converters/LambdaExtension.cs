using System;
using System.Linq.Dynamic;
using System.Linq.Expressions;
using System.Windows.Data;
using System.Windows.Markup;

namespace Viatronix.UI.Converters
{
    public static class Generic
    {
        public static TService GetService<TService>(this IServiceProvider isp) 
            where TService:class
        {
            return isp.GetService(typeof(TService))
                as TService;
        }
    }

    public class LambdaExtension : MarkupExtension
    {
        public string Forward { get; set; }
        public string Backward { get; set; }

        public LambdaExtension()
        {
        }

        public LambdaExtension(string forward)
        {
            this.Forward = forward;
        }

        public override object ProvideValue(IServiceProvider isp)
        {
            IProvideValueTarget tp = isp.GetService<IProvideValueTarget>();
            IXamlTypeResolver tr = isp.GetService<IXamlTypeResolver>();
            if (tp.TargetObject is Binding) return new SingleConverter(this.Forward, this.Backward, tr);
            if (tp.TargetObject is MultiBinding) return new MultiConverter(this.Forward, this.Backward, tr);
            if (tp.TargetObject is BindingExtension) return new MultiConverter(this.Forward, this.Backward, tr);
            return null;
        }
    }

    class SingleConverter : IValueConverter
    {
        public Delegate ForwardOperation { get; private set; }
        public Delegate BackwardOperation { get; private set; }
        public string Forward { get; private set; }
        public string Backward { get; private set; }
        private IXamlTypeResolver TypeResolver;

        public SingleConverter(string forward, string backward, IXamlTypeResolver tr)
        {
            this.Forward = forward;
            this.Backward = backward;
            this.TypeResolver = tr;
        }

        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (this.ForwardOperation == null)
            {
                if (value == null) return Binding.DoNothing;
                ForwardOperation = ConstructOperation(this.Forward, value, targetType);
            }
            return this.ForwardOperation.DynamicInvoke(value);
        }

        private Delegate ConstructOperation(string code, object value, Type targetType)
        {
            int opi = code.IndexOf("=>");
            if (opi < 0) throw new Exception("No lambda operator =>");
            string param = code.Substring(0, opi);
            string body = code.Substring(opi + 2);
            ParameterExpression p = Expression.Parameter(value.GetType(), param);
            LambdaExpression lambda = System.Linq.Dynamic.DynamicExpression.ParseLambda(this.TypeResolver, new ParameterExpression[] { p }, targetType, body, value);
            return lambda.Compile();
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (this.Backward == null) throw new Exception("No expression for ConvertBack");

            if (this.BackwardOperation == null)
            {
                if (value == null) return Binding.DoNothing;
                this.BackwardOperation = ConstructOperation(this.Backward, value, targetType);
            }
            return this.BackwardOperation.DynamicInvoke(value);
        }

        #endregion
    }

    class MultiConverter : IMultiValueConverter
    {
        public Delegate ForwardOperation { get; private set; }
        public Delegate BackwardOperation { get; private set; }
        public string Forward { get; private set; }
        public string Backward { get; private set; }
        private IXamlTypeResolver TypeResolver;

        public MultiConverter(string forward, string backward, IXamlTypeResolver tr)
        {
            this.Forward = forward;
            this.Backward = backward;
            this.TypeResolver = tr;
        }

        #region IMultiValueConverter Members

        public object Convert(object[] values, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (this.ForwardOperation == null)
            {
                int paramSideLength = this.Forward.IndexOf("=>");
                if (paramSideLength < 0) throw new Exception("No lambda operator =>");
                string paramSide = this.Forward.Substring(0, paramSideLength);
                string[] paramNames = paramSide.Split(',');
                string bodySide = this.Forward.Substring(paramSideLength + 2);
                ParameterExpression[] pes = new ParameterExpression[paramNames.Length];
                for (int i = 0; i < values.Length; i++)
                {
                    pes[i] = Expression.Parameter(values[i].GetType(), paramNames[i]);
                }
                LambdaExpression lambda = System.Linq.Dynamic.DynamicExpression.ParseLambda(TypeResolver, pes, targetType, bodySide, values);
                this.ForwardOperation = lambda.Compile();
            }
            return this.ForwardOperation.DynamicInvoke(values);
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, System.Globalization.CultureInfo culture)
        {
            if (this.BackwardOperation == null)
            {
                int opi = this.Backward.IndexOf("=>");
                if (opi < 0) throw new Exception("No lambda operator =>");
                string param = this.Backward.Substring(0, opi);
                string body = this.Backward.Substring(opi + 2);
                ParameterExpression p = Expression.Parameter(value.GetType(), param);
                LambdaExpression lambda = System.Linq.Dynamic.DynamicExpression.ParseLambda(this.TypeResolver, new ParameterExpression[] { p }, typeof(object[]), body, value);
                this.BackwardOperation = lambda.Compile();
            }
            return this.BackwardOperation.DynamicInvoke(value) as object[];
        }

        #endregion
    }
}
